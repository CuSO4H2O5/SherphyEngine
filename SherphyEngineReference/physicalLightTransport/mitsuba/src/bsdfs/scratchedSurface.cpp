/*
    This file is part of Mitsuba, a physically based rendering system.

    Copyright (c) 2007-2014 by Wenzel Jakob and others.

    Mitsuba is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    Mitsuba is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <mitsuba/render/scene.h>
#include <mitsuba/render/bsdf.h>
#include <mitsuba/render/sampler.h>
#include <mitsuba/core/warp.h>
#include <mitsuba/render/mipmap.h>
#include "ior.h"

#include <algorithm>
#include <vector>
#include <utility>
#include <random>

MTS_NAMESPACE_BEGIN

class ScratchedSurface : public BSDF {
private:
    typedef TMIPMap<TSpectrum<Float,1>, TSpectrum<Float,1>> MIPMap1;
    typedef TMIPMap<Spectrum, Spectrum> MIPMap;
    struct brdf_t {
        ref<MIPMap> brdf{};
        ref<MIPMap1> pdf{};
        std::vector<std::size_t> icdf;
    };
    using brdf_pair_t = std::pair<float,brdf_t>;

    template <typename Func>
    static void for_each_brdf_pixel(std::size_t w, std::size_t h, Func &&f) {
        for (std::size_t y=0; y<h; ++y) 
        for (std::size_t x=0; x<w; ++x) {
            const auto fx = ((x<<1)+1)/double(w)-1.;
            const auto fy = ((y<<1)+1)/double(h)-1.;
            if (fx*fx+fy*fy>1) continue;
            const auto cos_theta = std::sqrt(1-fx*fx-fy*fy);
            f(x,y,y*w+x,cos_theta);
        }
    }

    static void normalize_brdf(Bitmap &bitmap) {
        auto *data = (Spectrum*)bitmap.getData();
        
        double a{};
        std::size_t d{};
        for_each_brdf_pixel(bitmap.getWidth(), bitmap.getHeight(), [&](auto,auto,auto p,auto cos_theta) {
            a += data[p].getLuminance()*cos_theta;
            ++d;
        });
        a *= M_PI / double(d);
        for (std::size_t p=0; p<bitmap.getPixelCount(); ++p)
            data[p] /= a;
    }
    static auto create_pdf(const Bitmap &bitmap) {
        ref<Bitmap> pdf = new Bitmap(Bitmap::ELuminance,Bitmap::EFloat32,bitmap.getSize());

        const auto *data = (const Spectrum*)bitmap.getData();
        auto *target     = (float*)pdf->getData();
        std::fill(target,target+pdf->getPixelCount(),.0f);

        double accum{};
        for_each_brdf_pixel(pdf->getWidth(), pdf->getHeight(), [&](auto,auto,auto p,auto cos_theta) {
            accum += data[p].getLuminance()*cos_theta;
        });
        for_each_brdf_pixel(pdf->getWidth(), pdf->getHeight(), [&](auto,auto,auto p,auto cos_theta) {
            target[p] = float(data[p].getLuminance()*cos_theta/accum);
        });
        
        return pdf;
    }
    static auto create_icdf(const Bitmap &pdf) {
        const auto *vpdf = (const float*)pdf.getData();
        std::vector<std::size_t> vicdf;
        vicdf.resize(pdf.getPixelCount()+1);
        
        double accum{};
        std::size_t i=1;
        vicdf[0]=0;
        for (std::size_t p=0; p<pdf.getPixelCount(); ++p) {
            accum += vpdf[p];
            const auto j = std::min(pdf.getPixelCount(), 
                                    (std::size_t)(accum*pdf.getPixelCount()+.5f));
            for (;i<=j;++i)
                vicdf[i] = p;
        }
        for (; i<=pdf.getPixelCount(); ++i)
            vicdf[i] = pdf.getPixelCount()-1;
        
        return vicdf;
    }
    
public:
    ScratchedSurface(const Properties &props) : BSDF(props) {
        m_scale = props.getSpectrum("scale", Spectrum{ 1.0f });
        m_dist_multiplier = props.getFloat("distMultiplier", 1.0f);
        m_flipXY = props.getBoolean("flipXY", false);
    }

    ScratchedSurface(Stream *stream, InstanceManager *manager)
     : BSDF(stream, manager) {
        assert(false);
        // m_brdfs.clear();
        // const std::size_t count = stream->readUInt();
        // m_brdfs.reserve(count);
        // for (std::size_t i=0;i<count;++i) {
        // }
    
        configure();
    }
    void serialize(Stream *stream, InstanceManager *manager) const {
        assert(false);
        // stream->writeUInt(m_brdfs.size());
        // for (const auto &b : m_brdfs) {
        // }
    }
    
    void addChild(const std::string &name, ConfigurableObject *child) {
        if (child->getClass()->derivesFrom(MTS_CLASS(Texture))) {
            const Properties &props = child->getProperties();
            if (props.getPluginName() == "bitmap" && !props.hasProperty("gamma"))
                Log(EError, "When using a bitmap texture, please explicitly specify the 'gamma' parameter of the bitmap plugin. In most cases the following is the correct choice: <float name=\"gamma\" value=\"1.0\"/>");

            float dist = std::atof(name.data());
            if (dist<=.0f)
                Log(EError, "Name must be a positive distance.");
            
            auto brdf = static_cast<Texture *>(child);
            auto bitmap = brdf->getBitmap({});
            if (bitmap->getPixelFormat() != Bitmap::ESpectrum || bitmap->getComponentFormat() != Bitmap::EFloat32)
                bitmap = bitmap->convert(Bitmap::ESpectrum,Bitmap::EFloat32);
            if (m_flipXY)
                bitmap = bitmap->rotateFlip(Bitmap::ERotate90FlipNone);
            normalize_brdf(*bitmap);
            auto pdf  = create_pdf(*bitmap);
            auto icdf = create_icdf(*pdf);
            
            auto brdfmm = new  MIPMap(bitmap.get(), Bitmap::ESpectrum,  Bitmap::EFloat32, 
                                      nullptr, ReconstructionFilter::EZero, ReconstructionFilter::EZero, EBilinear, 1.f, fs::path(), 0);
            auto pdfmm  = new MIPMap1(pdf.get(),    Bitmap::ELuminance, Bitmap::EFloat32, 
                                      nullptr, ReconstructionFilter::EZero, ReconstructionFilter::EZero, EBilinear, 1.f, fs::path(), 0);

            auto item = brdf_pair_t{ 
                dist,
                brdf_t{ std::move(brdfmm), std::move(pdfmm), std::move(icdf) } 
            };
            const auto it = std::upper_bound(m_brdfs.begin(), m_brdfs.end(), item, [](const auto &o1, const auto &o2) {
                return o1.first<o2.first;
            });
            m_brdfs.insert(it,std::move(item));
        } else {
            BSDF::addChild(name, child);
        }
    }

    void configure() {
        if (!m_brdfs.size())
            Log(EError, "No BRDF textures were specified.");
        
        m_components.clear();
        m_components.push_back(EGlossyReflection | EFrontSide | ENonSymmetric | EDiffractive | ESpatiallyVarying | EAnisotropic);

        BSDF::configure();
    }
    
    inline Vector reflect(const Vector &wi, const Normal &n) const {
        return 2*dot(wi,n)*Vector(n) - wi;
    }
    inline auto refl_frame(const Vector &wi) const {
        const auto refl = reflect(wi, Normal{ 0,0,1 });
        const auto X = normalize(cross(refl,Vector{ 0,1,0 }));
        const auto Y = cross(refl,X);

#ifdef MTS_DEBUG
        if (EXPECT_NOT_TAKEN(!std::isfinite(Y.x) || !std::isfinite(Y.y) || !std::isfinite(Y.z))) {
            Log(EError, "refl_frame(): encountered a NaN!");
            return Frame{};
        }
#endif

        return Frame{ X,Y,refl };
    }
    
    struct eval_data_t {
        Point2 uv;
        Frame frame;
        const brdf_t* brdf1{}, *brdf2{};
        Float w;
    };
    auto eval_data(const BSDFSamplingRecord &bRec) const {
        auto s_over_rho = bRec.normalized_distance_travelled_from_emitter*m_dist_multiplier;
        if (EXPECT_NOT_TAKEN(!std::isfinite(s_over_rho) || s_over_rho<.0f)) {
            Log(EError, "eval_data(): encountered a NaN!");
            s_over_rho = .0f;
        }
        // if (s_over_rho<=.0f) 
        //     Log(EError, "Unrecorded distance from emitter.");
        
        const auto brdf2 = std::upper_bound(m_brdfs.begin(), m_brdfs.end(), brdf_pair_t{ s_over_rho, brdf_t{} }, 
                                            [](const auto &o1, const auto &o2) { return o1.first<o2.first; });
        const auto brdf1 = brdf2 != m_brdfs.begin() ? std::prev(brdf2) : m_brdfs.end();
        const auto frame = refl_frame(bRec.wi);
        const auto lookup_wo = frame.toLocal(bRec.wo);

        eval_data_t d{};
        d.frame = frame;
        d.uv    = Point2{ lookup_wo.x,lookup_wo.y }*Float(.5) + Point2{ .5,.5 };
        d.brdf1 = brdf1!=m_brdfs.end() ? &brdf1->second : nullptr;
        d.brdf2 = brdf2!=m_brdfs.end() ? &brdf2->second : nullptr;
        d.w     = d.brdf1 && d.brdf2 ? 
                    (s_over_rho-brdf1->first)/(brdf2->first-brdf1->first) : 
                    1.f;
        return d;
    }
    Point2 sample_uv(const eval_data_t &d, const Point2 &sample) const {
        const auto *brdf = !d.brdf1 ? d.brdf2 : 
                           !d.brdf2 ? d.brdf1 :
                           d.w>=Float(.5) ? d.brdf2 : d.brdf1;
        const auto *icdf = &brdf->icdf;
        const auto w = brdf->brdf->getWidth();
        const auto h = brdf->brdf->getHeight();

        const auto i = sample.x*icdf->size();
        const auto f = i-(std::size_t)i;
        const auto b = std::min((std::size_t)i, icdf->size()-1);
        const auto t = std::min(b+1, icdf->size()-1);
        const auto p = (std::size_t)((1-f)*(*icdf)[b]+f*(*icdf)[t]+.5f);
        
        const auto x = Float(p%w);
        const auto y = Float(p/w);

        auto uv = Point2{ x/w,y/h };

        static constexpr Float pertrub_pixels = 1;
        double s,c;
        sincos(2*M_PI*sample.y,&s,&c);
        const auto pd = (sample.x+sample.y)/2;
        uv += Vector2{ Float(c)/w,Float(s)/h }*pd*pd * pertrub_pixels;
        
        return Point2(uv);
    }
    Spectrum eval(const BSDFSamplingRecord &bRec, const eval_data_t &d) const {
        const auto brdf1val = d.brdf1 ? d.brdf1->brdf->evalBilinear(0,d.uv) : Spectrum(.0f);
        const auto brdf2val = d.brdf2 ? d.brdf2->brdf->evalBilinear(0,d.uv) : Spectrum(.0f);
        
        const auto val = !d.brdf1 ? brdf2val : 
                         !d.brdf2 ? brdf1val :
                         Spectrum((1-d.w)*brdf1val + d.w*brdf2val);
        
        // Spectrum ret;
        // ret.fromLinearRGB(1,0,0);
        // if (bRec.normalized_distance_travelled_from_emitter == 0)
        //     return ret;
        
        return val * m_scale;
    }
    Float pdf(const BSDFSamplingRecord &bRec, const eval_data_t &d) const {
        const auto bm = !d.brdf1 ? d.brdf2->pdf.get() : 
                        !d.brdf2 ? d.brdf1->pdf.get() :
                        d.w>=Float(.5) ? d.brdf2->pdf.get() : d.brdf1->pdf.get();

        return bm->evalBilinear(0,d.uv)[0];
    }

    Spectrum eval(const BSDFSamplingRecord &bRec, EMeasure measure) const {
        if (measure != ESolidAngle ||
            Frame::cosTheta(bRec.wi) <= 1e-10f || Frame::cosTheta(bRec.wo) <= 1e-10f)
            return Spectrum(.0f);
        
        return eval(bRec,eval_data(bRec));
    }

    Float pdf(const BSDFSamplingRecord &bRec, EMeasure measure) const {
        if (measure != ESolidAngle ||
            !(bRec.typeMask & EGlossyReflection) ||
            (bRec.component != -1 && bRec.component != 0) ||
            Frame::cosTheta(bRec.wi) <= 1e-10f || Frame::cosTheta(bRec.wo) <= 1e-10f)
            return .0f;
        
        return pdf(bRec,eval_data(bRec));
    }

    Spectrum sample(BSDFSamplingRecord &bRec, const Point2 &sample) const {
        Float unused;
        return this->sample(bRec, unused, sample);
    }

    Spectrum sample(BSDFSamplingRecord &bRec, Float &vpdf, const Point2 &sample) const {
        if (!(bRec.typeMask & EGlossyReflection) ||
            (bRec.component != -1 && bRec.component != 0) ||
            Frame::cosTheta(bRec.wi) <= 1e-10f)
            return Spectrum(.0f);

        auto wo = bRec.wo;
        eval_data_t d;
        
        d = eval_data(bRec);
        d.uv = sample_uv(d, sample);
        const auto xy = d.uv*2.f - Point2{ 1,1 };
        const auto sampled_wo = Vector{ xy.x,xy.y,std::sqrt(1-std::min(Float(1),xy.x*xy.x+xy.y*xy.y)) };
        wo = d.frame.toWorld(sampled_wo);
            
#ifdef MTS_DEBUG
        if (EXPECT_NOT_TAKEN(!std::isfinite(wo.x) || !std::isfinite(wo.y) || !std::isfinite(wo.z))) {
            Log(EError, "sample(): encountered a NaN!");
            return Spectrum(.0f);
        }
#endif
        
        if (Frame::cosTheta(bRec.wo)<=0)
            return Spectrum(.0f);

        vpdf = pdf(bRec,d);
        if (vpdf == 0)
            return Spectrum(.0f);

        bRec.wo = wo;
        bRec.eta = 1.0f;
        bRec.sampledComponent = 0;
        bRec.sampledType = EGlossyReflection;

        return eval(bRec,d) / vpdf;
    }

    std::string toString() const {
        std::ostringstream oss;
        oss << "ScratchedSurface[" << endl
            << "  id = \"" << getID() << "\"" << endl
            << "]";
        return oss.str();
    }

    MTS_DECLARE_CLASS()
private:
    Float m_dist_multiplier;
    bool m_flipXY{};
    Spectrum m_scale;
    std::vector<brdf_pair_t> m_brdfs;
};

MTS_IMPLEMENT_CLASS_S(ScratchedSurface, false, BSDF)
MTS_EXPORT_PLUGIN(ScratchedSurface, "Scratched Surface Diffractive BSDF");
MTS_NAMESPACE_END

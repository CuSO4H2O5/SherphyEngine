
# notations
https://en.wikipedia.org/wiki/Notation_for_differentiation  
下面的记法称为：Leibniz's notation  
$$ \textbf{a} = \frac{d^2x}{dt^2}$$
newton's notation  
$$ \textbf{v} =  \dot{x} $$
<!-- \overset{.}{x} -->


# 欧拉积分
时步约定符号为
$$ h $$
基本假设： 模拟过程中积分的所有值在该时步内保持不变


永远选取更好的积分方法来得到准确解


# 碰撞
碰撞三阶段：
1. 碰撞检测（collision detection）
2. 碰撞测定（collision determination） 碰撞在什么时候以及什么地方出现
3. 碰撞响应（collision response）碰撞产生什么效果

## 碰撞检测
$$ (\textbf{x} - \textbf{p}) \cdot \hat{\textbf{n}} = 0 \hspace{10mm} (1) $$
x 为检测顶点  
p 为平面上一点  
n 平面法线向量  
<b>带有约束的碰撞检测</b>  
修改公式为：
$$ d = (\textbf{x} - \textbf{p}) \cdot \hat{\textbf{n}} + \textbf{f(x)} \hspace{10mm} (2)$$
其中在为球时，f(x) = r
利用根存在性定理
$$ f(a)f(b)<0 $$

## 碰撞测定
<b>bias 问题</b>
在时步的分数计算时，由于时步使用浮点表示，因此容易被舍入为0，导致TimeStepRemaining无法为0。
可以设置使用\epsilon为一相较timestep时长小几个数量级的值进行比较。
使用定点数来提高精度也是一种方式。


## 碰撞响应


# 数值精度
数值误差 numerical error  
修约误差 round-off error  
积分误差 integration error  
几何离散化  

公差 tolerance  
公差无法实现传递性，如：  
A = 1.7  
B = 2.0  
C = 2.3  
当公差为0.3时  
A=B  
B=C  
因此 A=B=C  
这显然是错误的
因此在公式设计以及架构上应尽量避免公差传递

# 静止条件
跳转第九章刚体力平衡



# 多边形polygon
简单多边形：  
一个有序顶点集合，全部顶点在同一个平面上，并且按序链接顶点会组成一个无棱相交的闭环。  

四边形建模对于建模师来说是一件愉快的事情，然而对于渲染来说四边形允许四个点不在同一个平面的性质并不良好。  
由此利用三角形性质计算法线即可  该内容略

polygon的内外检测有一个较好的性质：  
在polygon内的点在投影到一个平面后依旧在polygon的投影内，由此可以利用坐标系投影简化计算。  

利用重心坐标可以较快判断内外关系


该部分参考103第一章节制作即可

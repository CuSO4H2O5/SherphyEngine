set(TARGET_PROJECT Sherphy_Lan)

file(GLOB Lan_HEADERS "*.h")
file(GLOB Lan_SOURCES "*.cpp")

add_executable(${TARGET_PROJECT} ${Lan_HEADERS} ${Lan_SOURCES})

set_target_properties(${TARGET_PROJECT} PROPERTIES FOLDER "GamePlay")

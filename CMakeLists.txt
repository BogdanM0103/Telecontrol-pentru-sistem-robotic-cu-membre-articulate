cmake_minimum_required(VERSION 3.25)
project(HexapodRobot LANGUAGES CXX)

# —————————————————————————————————————————————————————————————————————
# 1) User-editable deploy settings
#    You can override these in CLion's CMake settings or on the command line:
#      -DDEPLOY_HOST=192.168.1.42   -DDEPLOY_USER=pi   -DDEPLOY_PATH=/home/pi/HexapodRobot
#    Leave DEPLOY_HOST empty to disable auto-deploy.
set(DEPLOY_HOST "" CACHE STRING "Hostname or IP of your Raspberry Pi")
set(DEPLOY_USER "pi" CACHE STRING "SSH user for your Raspberry Pi")
set(DEPLOY_PATH "/home/pi/HexapodRobot" CACHE STRING "Remote path on Pi to copy into")
# —————————————————————————————————————————————————————————————————————

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include your headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# Build the hexapod executable
add_executable(HexapodRobot
        main.cpp
        Serial.cpp
        Kinematics.cpp
        Servo.cpp
        LegCoordinates.cpp
        Gait.cpp
)

# —————————————————————————————————————————————————————————————————————
# 2) Post-build deploy: scp the binary + Python scripts if DEPLOY_HOST is set
if(DEPLOY_HOST)
    add_custom_command(TARGET HexapodRobot POST_BUILD
            COMMENT "→ Deploying HexapodRobot + Python scripts to ${DEPLOY_USER}@${DEPLOY_HOST}:${DEPLOY_PATH}"
            # ensure target directory exists
            COMMAND ssh ${DEPLOY_USER}@${DEPLOY_HOST}
            "mkdir -p ${DEPLOY_PATH}"
            # copy the binary and Python scripts
            COMMAND scp $<TARGET_FILE:HexapodRobot>
            ${CMAKE_SOURCE_DIR}/server.py
            ${CMAKE_SOURCE_DIR}/udp_server.py
            ${DEPLOY_USER}@${DEPLOY_HOST}:${DEPLOY_PATH}/
    )
endif()
# —————————————————————————————————————————————————————————————————————

# (Optional) enable `make install` for a local install/ folder
install(TARGETS HexapodRobot
        RUNTIME DESTINATION bin
)
install(FILES ${CMAKE_SOURCE_DIR}/server.py
        ${CMAKE_SOURCE_DIR}/udp_server.py
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
        GROUP_READ GROUP_EXECUTE
        WORLD_READ WORLD_EXECUTE
        DESTINATION bin
)
include(CPack)

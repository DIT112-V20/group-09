include (Patcher)

# Custom GY50 impl
SET(GY50 ${CMAKE_SOURCE_DIR}/thirdparty/smartcar_shield/src/sensors/heading/gyroscope/GY50.cpp)
hard_patch_regexp(${GY50} "void GY50::update\(\).*currentTime;\n}" "void GY50::update() { mAngularDisplacement = getAngularVelocity(); }")
hard_patch_regexp(${GY50} "int GY50::getHeading\(\).*normalizedReading;\n}" "int GY50::getHeading() { return mAngularDisplacement; }")
hard_patch_regexp(${GY50} "int GY50::getOffset.*measurements\n};" "int GY50::getOffset(int measurements) { return 0; }")

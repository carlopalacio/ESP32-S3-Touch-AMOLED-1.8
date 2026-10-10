idf_component_register(SRCS "main.c" "counter.c" INCLUDE_DIRS "." REQUIRES esp32_s3_touch_amoled_1_8 lvgl__lvgl)

idf_build_get_property(build_components BUILD_COMPONENTS)
if("lvgl__lvgl" IN_LIST build_components)
    idf_component_get_property(LVGL_LIB lvgl__lvgl COMPONENT_LIB)
    if(TARGET ${LVGL_LIB})
        target_compile_options(${LVGL_LIB} PRIVATE -Wno-error=attributes)
    endif()
endif()

#*******************************************************************************
#  Copyright (c) 2017 IBM Corp.
#
#  All rights reserved. This program and the accompanying materials
#  are made available under the terms of the Eclipse Public License v1.0
#  and Eclipse Distribution License v1.0 which accompany this distribution.
#
#  The Eclipse Public License is available at
#     http://www.eclipse.org/legal/epl-v10.html
#  and the Eclipse Distribution License is available at
#    http://www.eclipse.org/org/documents/edl-v10.php.
#
#  Contributors:
#     Ian Craggs - initial version
#*******************************************************************************/

# MQTTClient Library - C


file(GLOB SOURCES "*.c" "FreeRTOS/*.c")

add_library(
  paho-lwip-mqtt3cc STATIC
  ${SOURCES}
)
install(TARGETS paho-lwip-mqtt3cc DESTINATION /usr/lib)
target_include_directories(paho-lwip-mqtt3cc PRIVATE "FreeRTOS" ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries( paho-lwip-mqtt3cc paho-lwip-mqtt3c pico_stdlib FreeRTOS-Kernel LWIP_PORT pico_cyw43_arch_lwip_sys_freertos)
target_compile_definitions(paho-lwip-mqtt3cc PRIVATE
             MQTTCLIENT_PLATFORM_HEADER=MQTTFreeRTOS.h MQTTCLIENT_QOS2=1)

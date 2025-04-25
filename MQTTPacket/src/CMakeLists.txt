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

# MQTTPacket Library
file(GLOB SOURCES "*.c")
add_library(paho-lwip-mqtt3c INTERFACE ${SOURCES})
install(TARGETS paho-lwip-mqtt3c DESTINATION /usr/lib)
target_include_directories(paho-lwip-mqtt3c INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_compile_definitions(paho-lwip-mqtt3c INTERFACE MQTT_SERVER MQTT_CLIENT)

add_library(MQTTPacketClient INTERFACE MQTTFormat MQTTPacket
            MQTTSerializePublish MQTTDeserializePublish
            MQTTConnectClient MQTTSubscribeClient MQTTUnsubscribeClient)
target_compile_definitions(MQTTPacketClient INTERFACE MQTT_CLIENT)

add_library(MQTTPacketServer INTERFACE MQTTFormat MQTTPacket
            MQTTSerializePublish MQTTDeserializePublish
            MQTTConnectServer MQTTSubscribeServer MQTTUnsubscribeServer)
target_compile_definitions(MQTTPacketServer INTERFACE MQTT_SERVER)



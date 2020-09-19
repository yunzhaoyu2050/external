/*
 * Copyright (c) 2017 Baidu, Inc. All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include "DCSApp/ActivityMonitorSingleton.h"
#include <unistd.h>
#define SECS_PER_MIN 60
#define JSON_FILE_NAME "./sdk_monitor.json"
namespace duerOSDcsApp {
namespace application {
ActivityMonitorSingleton* ActivityMonitorSingleton::s_instance = nullptr;
pthread_once_t ActivityMonitorSingleton::s_init_once = PTHREAD_ONCE_INIT;
pthread_once_t ActivityMonitorSingleton::s_destroy_once = PTHREAD_ONCE_INIT;
ActivityMonitorSingleton* ActivityMonitorSingleton::getInstance() {
    pthread_once(&s_init_once, &ActivityMonitorSingleton::init);
    return s_instance;
}
void ActivityMonitorSingleton::releaseInstance() {
    pthread_once(&s_destroy_once, ActivityMonitorSingleton::destory);
}
ActivityMonitorSingleton::ActivityMonitorSingleton() : m_threadAlive(true) {
    pthread_mutex_init(&m_mutex, NULL);
    pthread_create(&m_threadId, nullptr, threadRoutine, this);
}
ActivityMonitorSingleton::~ActivityMonitorSingleton() {
    pthread_mutex_destroy(&m_mutex);
    m_threadAlive = false;
}
void ActivityMonitorSingleton::init() {
    if (nullptr == s_instance) {
        s_instance = new ActivityMonitorSingleton();
    }
}
void ActivityMonitorSingleton::destory() {
    if (s_instance) {
        delete s_instance;
        s_instance = nullptr;
    }
}
void ActivityMonitorSingleton::updatePlayerStatus(PlayerStatus playerStatus) {
    pthread_mutex_lock(&m_mutex);
    m_monitorItem.m_playerStatus = playerStatus;
    m_monitorItem.m_timestamp = 0;
    pthread_mutex_unlock(&m_mutex);
    updateJsonFile();
}
void ActivityMonitorSingleton::updateJsonFile() {
    pthread_mutex_lock(&m_mutex);
    std::string targetStr;
    if (m_monitorItem.m_playerStatus == PLAYER_STATUS_ON) {
        targetStr = "0";
    } else {
        targetStr = m_monitorItem.m_timestamp >= 30 ? "1" : "0";
    }
    if (m_prevWriteContent == targetStr) {
        pthread_mutex_unlock(&m_mutex);
        return;
    }
    m_prevWriteContent = targetStr;

    rapidjson::Document document;
    document.SetObject();
    addPairToDoc(document, "device_idle", targetStr);
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    const char* str = buffer.GetString();
    FILE *fp = fopen(JSON_FILE_NAME , "w");
    if (!fp) {
        pthread_mutex_unlock(&m_mutex);
        return;
    }
    fwrite(str, sizeof(char), strlen(str), fp);
    fclose(fp);
    sync();
    pthread_mutex_unlock(&m_mutex);
}
void ActivityMonitorSingleton::addPairToDoc(rapidjson::Document& document,
                                            const std::string& key,
                                            const std::string& value) {
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value pKey(rapidjson::kStringType);
    pKey.SetString(key, allocator);
    rapidjson::Value pValue(rapidjson::kStringType);
    pValue.SetString(value, allocator);
    document.AddMember(pKey, pValue, allocator);
}
void* ActivityMonitorSingleton::threadRoutine(void *arg) {
    ActivityMonitorSingleton *instance = (ActivityMonitorSingleton *) arg;
    instance->updateJsonFile();
    while (instance->m_threadAlive) {
        sleep(SECS_PER_MIN);
        instance->m_monitorItem.m_timestamp++;
        instance->updateJsonFile();
    }
    return nullptr;
}
}  // namespace application
}  // namespace duerOSDcsApp

/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>
#include <fstream>
#include <string>

#include <mesos/hook.hpp>
#include <mesos/mesos.hpp>
#include <mesos/module.hpp>

#include <mesos/module/hook.hpp>

#include <process/future.hpp>
#include <process/process.hpp>
#include <process/protobuf.hpp>

#include <stout/foreach.hpp>
#include <stout/os.hpp>
#include <stout/try.hpp>

#include <stout/result.hpp>

#include <curl/curl.h>

using namespace mesos;

using process::Future;

using std::map;
using std::string;


static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}


class VaultHook : public Hook
{
public:
  // In this hook, we create a new environment variable "FOO" and set
  // it's value to "bar".
  // works: runs
//  virtual Result<Environment> slaveExecutorEnvironmentDecorator(
//      const ExecutorInfo& executorInfo)
//  {
  virtual process::Future<Option<Environment>>
    slavePreLaunchDockerEnvironmentDecorator(
        const Option<TaskInfo>& taskInfo,
        const ExecutorInfo& executorInfo,
        const std::string& name,
        const std::string& sandboxDirectory,
        const std::string& mappedDirectory,
        const Option<std::map<std::string, std::string>>& env)
  {
    LOG(INFO) << "Executing 'slaveExecutorEnvironmentDecorator' hook";
    LOG(INFO) << "ExecutorID: " << executorInfo.executor_id().value() << ". Name: " << executorInfo.name() << ". CommandInfoValue: " << executorInfo.command().value() << ".";
    LOG(INFO) << "Name: " << name << ". TaskInfo.name: " << taskInfo.get().name() << ". taskInfo.taskId: " << taskInfo.get().task_id().value();

    std::string token;
    std::ifstream myfile ("/root/.vault-token");
    if (myfile.is_open()) {
      getline(myfile, token);
      myfile.close();
    }
    else token = "NOTOKEN";

    LOG(INFO) << "TOKEN: " << token;

    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    std::string vault_header = "X-Vault-Token: " + token;

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if (curl) {
      struct curl_slist *token_header = NULL;
      token_header = curl_slist_append(token_header, vault_header.c_str());
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, token_header);
      curl_easy_setopt(curl, CURLOPT_URL, "https://169.254.255.254:20161/v1/services/?list=true");
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
      res = curl_easy_perform(curl);
      if (res != CURLE_OK)
        LOG(INFO) << "CURL DIDN'T WORK";
      else
        LOG(INFO) << "CURL WORKED";
      LOG(INFO) << "Curl output: " << readBuffer;
      curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    Environment environment;

    if (executorInfo.command().has_environment()) {
      environment.CopyFrom(executorInfo.command().environment());
    }

    Environment::Variable* variable = environment.add_variables();
    variable->set_name("FOO");
    variable->set_value("bar");

    return environment;
  }

};


static Hook* createHook(const Parameters& parameters)
{
  return new VaultHook();
}


// Declares a Hook module named 'org_apache_mesos_TestHook'.
mesos::modules::Module<Hook> com_yelp_VaultHook(
    MESOS_MODULE_API_VERSION,
    MESOS_VERSION,
    "Matthew Bentley",
    "bentley@yelp.com",
    "Vault integration hook.",
    NULL,
    createHook);

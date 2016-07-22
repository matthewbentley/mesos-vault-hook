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

#include <stout/result.hpp>

using namespace mesos;
using namespace std;

using std::map;
using std::string;


class VaultHook : public Hook
{
public:
  // In this hook, we create a new environment variable "FOO" and set
  // it's value to "bar".
  // works: runs
  virtual Result<Environment> slaveExecutorEnvironmentDecorator(
      const ExecutorInfo& executorInfo)
  {
    LOG(INFO) << "Executing 'slaveExecutorEnvironmentDecorator' hook";
    LOG(INFO) << "ExecutorID: " << executorInfo.executor_id().value() << ". Name: " << executorInfo.name() << ". CommandInfoValue: " << executorInfo.command().value() << ".";

    string token;
    ifstream myfile ("/root/.vault-token");
    if (myfile.is_open()) {
      getline(myfile, token);
      myfile.close();
    }
    else token = "NOTOKEN";

    LOG(INFO) << "TOKEN: " << token;

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

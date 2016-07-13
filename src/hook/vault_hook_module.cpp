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

#include <mesos/hook.hpp>
#include <mesos/mesos.hpp>
#include <mesos/module.hpp>

#include <mesos/module/hook.hpp>

#include <process/protobuf.hpp>

#include <process/future.hpp>

#include <stout/none.hpp>
#include <stout/nothing.hpp>
#include <stout/result.hpp>
#include <stout/try.hpp>
#include <stout/option.hpp>

using namespace mesos;

using process::Future;

class VaultHook : public Hook
{
public:
//  // In this hook, we create a new environment variable "FOO" and set
//  // it's value to "bar".
//  virtual Result<Environment> slaveExecutorEnvironmentDecorator(
//      const ExecutorInfo& executorInfo)
//  {
//    LOG(INFO) << "Executing 'slaveExecutorEnvironmentDecorator' hook";
//    LOG(INFO) << "Name: " << executorInfo.name();
//
//    Environment environment;
//
//    if (executorInfo.command().has_environment()) {
//      environment.CopyFrom(executorInfo.command().environment());
//    }
//
//    Environment::Variable* variable = environment.add_variables();
//    variable->set_name("FOO");
//    variable->set_value("bar");
//
//    for(int i=0; i<environment.variables_size(); i++) {
//      LOG(INFO) << "ENV: " << environment.variables(i).name() << ":" << environment.variables(i).value();
//    }
//
//    return environment;
//  }
//
//  virtual Try<Nothing> slavePostFetchHook(
//      const ContainerID& containerId,
//      const std::string& directory)
//  {
//    LOG(INFO) << "Executing 'slavePostFetchHook' hook";
////    LOG(INFO) << "ContainerID: " << containerId.value() << ". directory: " << directory;
//
//    return Nothing();
//  }
//
  virtual process::Future<Option<Environment>>
    slavePreLaunchDockerEnvironmentDecorator(
        const Option<TaskInfo>& taskInfo,
        const ExecutorInfo& executorInfo,
        const std::string& name,
        const std::string& sandboxDirectory,
        const std::string& mappedDirectory,
        const Option<std::map<std::string, std::string>>& env)
  {
    LOG(INFO) << "Running 'slavePreLaunch...' hook";

    Environment environment;

    if (executorInfo.command().has_environment()) {
      environment.CopyFrom(executorInfo.command().environment());
    }

    Environment::Variable* variable = environment.add_variables();
    variable->set_name("VAULT_TOKEN");
    variable->set_value("foobar");
//    if (taskInfo.isSome()) {
//      LOG(INFO) << "hook taskInfo name: " << taskInfo.get().name() << ". taskInfo id: " << taskInfo.get().task_id().value() << ". ContainerInfo: " << taskInfo.get().container().docker().image();
//    }
//
//    LOG(INFO) << "hook name: " << name << ". sandboxdir: " << sandboxDirectory << ". mappeddir: " << mappedDirectory;

    return environment;
  }

//  virtual Try<Nothing> slavePreLaunchDockerHook(
//      const ContainerInfo& containerInfo,
//      const CommandInfo& commandInfo,
//      const Option<TaskInfo>& taskInfo,
//      const ExecutorInfo& executorInfo,
//      const std::string& name,
//      const std::string& sandboxDirectory,
//      const std::string& mappedDirectory,
//      const Option<Resources>& resources,
//      const Option<std::map<std::string, std::string>>& env)
//  {
//    LOG(INFO) << "Running slavePreLaunchDockerHook hook";
//    return Nothing();
//  }
//
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

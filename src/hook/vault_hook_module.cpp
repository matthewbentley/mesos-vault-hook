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
#include <process/process.hpp>
#include <process/future.hpp>

#include <stout/none.hpp>
#include <stout/nothing.hpp>
#include <stout/result.hpp>
#include <stout/try.hpp>
#include <stout/option.hpp>
#include <stout/foreach.hpp>
#include <stout/os.hpp>

using namespace mesos;

using std::map;
using std::string;

using process::Failure;
using process::Future;

const char* testLabelKey = "MESOS_Test_Label";
const char* testLabelValue = "ApacheMesos";
const char* testRemoveLabelKey = "MESOS_Test_Remove_Label";
const char* testErrorLabelKey = "MESOS_Test_Error_Label";


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

    Environment environment;

    if (executorInfo.command().has_environment()) {
      environment.CopyFrom(executorInfo.command().environment());
    }

    Environment::Variable* variable = environment.add_variables();
    variable->set_name("FOO");
    variable->set_value("bar");

    return environment;
  }


  // In this hook, look for the existence of a specific label.
  // If found, return a `Failure`.
  // Otherwise, add an environment variable to the task.
  // works: no
  virtual Future<Option<Environment>> slavePreLaunchDockerEnvironmentDecorator(
      const Option<TaskInfo>& taskInfo,
      const ExecutorInfo& executorInfo,
      const string& name,
      const string& sandboxDirectory,
      const string& mappedDirectory,
      const Option<map<string, string>>& env)
  {
    LOG(INFO) << "Executing 'slavePreLaunchDockerEnvironmentDecorator' hook";

    if (taskInfo.isSome()) {
      foreach (const Label& label, taskInfo->labels().labels()) {
        if (label.key() == testErrorLabelKey) {
          return Failure("Spotted error label");
        }
      }
    }

    Environment environment;
    Environment::Variable* variable = environment.add_variables();
    variable->set_name("FOO_DOCKER");
    variable->set_value("docker_bar");

    return environment;
  }


  // works: runs
  virtual Try<Nothing> slavePreLaunchDockerHook(
      const ContainerInfo& containerInfo,
      const CommandInfo& commandInfo,
      const Option<TaskInfo>& taskInfo,
      const ExecutorInfo& executorInfo,
      const string& name,
      const string& sandboxDirectory,
      const string& mappedDirectory,
      const Option<Resources>& resources,
      const Option<map<string, string>>& env)
  {
    LOG(INFO) << "Executing 'slavePreLaunchDockerHook' hook";
    return os::touch(path::join(sandboxDirectory, "foo"));
  }


  // works: no
  virtual Try<Nothing> slavePostFetchHook(
      const ContainerID& containerId,
      const string& directory)
  {
    LOG(INFO) << "Executing 'slavePostFetchHook' hook";

    const string path = path::join(directory, "post_fetch_hook");

    if (os::exists(path)) {
      return os::rm(path);
    } else {
      return Nothing();
    }
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

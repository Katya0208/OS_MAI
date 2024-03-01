#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "graph.h"
#include "parser.h"

using namespace std;

map<int, string> CreateDictionary(vector<Configuration> configs) {
  map<int, string> paths_dictionary;  // словарь <id jobа>:<исполняемый файл>
  for (auto config : configs) {
    paths_dictionary[config.id] = config.path;
  }
  return paths_dictionary;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    cout << "Error: Not input ini file!\n";
    exit(1);
  }

  vector<Configuration> configs;

  configs = IniParser(argv[1]);

  Graph dag_jobs = CreateGraph(configs);
  if (dag_jobs.isEmpty()) {
    cout << "The graph is empty!\n";
    exit(2);
  }

  // dag_jobs.print();

  vector<int> visited(dag_jobs.getSize(), 0);

  for (auto end_job : dag_jobs.end_jobs_id) {
    if (IsCycle(end_job, dag_jobs, visited)) {
      cout << "The graph has a cycle!\n";
      exit(3);
    }
  }

  vector<int> traveled(dag_jobs.getSize(), 0);
  vector<int> path = BFS(dag_jobs, traveled);

  map<int, string> job_paths = CreateDictionary(configs);

  int string_size = path.size() + 1;

  char state[string_size];
  for (int i = 0; i < string_size; ++i) {
    state[i] = '0';
  }

  for (int id = path.size() - 1; id >= 0; --id) {
    int pipe_parent[2];
    int pipe_child[2];
    if (pipe(pipe_parent) == -1) {
      fprintf(stderr, "Pipe Failed");
      exit(1);
    }

    if (pipe(pipe_child) == -1) {
      fprintf(stderr, "Pipe Failed");
      exit(1);
    }
    pid_t process_id = fork();

    if (process_id < 0) {
      printf("Error: Fork\n");
      exit(1);
    }

    if (process_id > 0) {
      close(pipe_parent[0]);

      write(pipe_parent[1], state, sizeof(char) * (string_size + 1));
      close(pipe_parent[1]);

      wait(NULL);

      close(pipe_child[1]);

      read(pipe_child[0], state, sizeof(char) * (string_size + 1));

      close(pipe_child[0]);

    } else {
      close(pipe_parent[1]);

      if (dup2(pipe_child[1], 2) == -1) {
        printf("Error: Dup2\n");
        exit(2);
      }

      char received[string_size];

      read(pipe_parent[0], received, sizeof(char) * (string_size + 1));

      close(pipe_parent[0]);

      close(pipe_child[0]);

      string adjacency;
      for (int i = 0; i < dag_jobs.adjacency[path[id]].size(); ++i) {
        adjacency.push_back(dag_jobs.adjacency[path[id]][i] + '0');
      }
      char ID[2];
      sprintf(ID, "%d", path[id]);

      execl(job_paths[path[id]].c_str(), job_paths[path[id]].c_str(), ID,
            received, adjacency.c_str(), NULL);
    }
  }

  for (int id = 1; id < dag_jobs.getSize(); ++id) {
    cout << "Result for job_id_" << id << " is " << state[id] << '\n';
  }

  return 0;
}

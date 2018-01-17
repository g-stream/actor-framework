#!/usr/bin/env groovy

def clang_builds = ["Linux && clang && LeakSanitizer",
                    "macOS && clang"]
def gcc_builds = ["Linux && gcc4.8",
                  "Linux && gcc4.9",
                  "Linux && gcc5.1",
                  "Linux && gcc6.3",
                  "Linux && gcc7.2"]
def msvc_builds = ["msbuild"]

pipeline {
  agent none

  stages {
    stage ('Get') {
      steps {
        node ('master') {
          // TODO: pull github branch into mirror
          // TODO: set URL, refs, prNum?
          echo "Hello from master"
        }
      }
    }
    stage ('Build') {
      parallel {
        stage ("Linux && gcc4.8") {
          agent { label "Linux && gcc4.8" }
          steps { do_stuff("Linux && gcc4.8") }
        }
        stage ("Linux && gcc4.9") {
          agent { label "Linux && gcc4.9" }
          steps { do_stuff("Linux && gcc4.9") }
        }
        stage ("macOS && clang") {
          agent { label "macOS && clang" }
          steps { do_stuff("macOS && clang") }
        }
      }
    }
    stage ('Test') {
      steps {
        // execute unit tests?
        echo "Testing all the things"
      }
    }
  }
}

if (currentBuild.result == null) {
  currentBuild.result = 'SUCCESS'
}

def do_stuff(tags,
              build_type = "Debug",
              generator = "Unix Makefiles",
              cmake_opts = "",
              build_opts = "") {
    echo "Starting build with '${tags}'"
    echo "Checkout"
    // TODO: pull from mirror, not from GitHub, (RIOT fetch func?)
    checkout scm
    echo "DEBUG INFO"
    sh 'git branch'
    sh 'ifconfig'
    echo "Configure"
    sh '''
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE="\${build_type}" -G "\${generator}" ..
    '''
    currentBuild.result
    echo "Build"
    // make -j 2 ${build_opts}
    echo "Test"
    // ctest .
}
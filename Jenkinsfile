#!groovy

parallel (

"nyu_VALGRIND" : {node ('nyu')
                  {
                    deleteDir()

                    int ntry = 5
                    while (ntry != 0)
                    {
                      try {
                        checkout scm
                        ntry = 0
                      }
                      catch (IOException e)
                      {
                        ntry--
                        sleep(10)
                      }
                    }

                    stage ('build_nyu_val')
                    {
                      sh "./build.sh $WORKSPACE $NODE_NAME VALGRIND $BRANCH_NAME"
                    }

                    stage ('run_nyu_val')
                    {
                      sh "cd openfpm_data && ./run.sh $WORKSPACE $NODE_NAME VALGRIND"
                      sh "cd openfpm_data && ./success.sh 1 nyu openfpm_data valgrind"
                    }
                  }
                 },


"nyu_NO" : {node ('nyu')
                  {
                    deleteDir()

                    int ntry = 5
                    while (ntry != 0)
                    {
                      try {
                        checkout scm
                        ntry = 0
                      }
                      catch (IOException e)
                      {
                        ntry--
                        sleep(10)
                      }
                    }

                    stage ('build_nyu_nor')
                    {
                      sh "./build.sh $WORKSPACE $NODE_NAME NO $BRANCH_NAME"
                    }

                    stage ('run_nyu_nor')
                    {
                      sh "cd openfpm_data && ./run.sh $WORKSPACE $NODE_NAME NO"
                      sh "cd openfpm_data && ./success.sh 2 nyu openfpm_data"
                    }
                  }
                 },

"nyu_SE" : {node ('nyu')
                  {
                    deleteDir()

                    int ntry = 5
                    while (ntry != 0)
                    {
                      try {
                        checkout scm
                        ntry = 0
                      }
                      catch (IOException e)
                      {
                        ntry--
                        sleep(10)
                      }
                    }

                    stage ('build_nyu_se')
                    {
                      sh "./build.sh $WORKSPACE $NODE_NAME SE $BRANCH_NAME"
                    }

                    stage ('run_nyu_se')
                    {
                      sh "cd openfpm_data && ./run.sh $WORKSPACE $NODE_NAME SE"
                      sh "cd openfpm_data && ./success.sh 1 nyu openfpm_data \"security enhancements\""
                    }
                  }
                 },

"sb15_VALGRIND" : {node ('sbalzarini-mac-15')
                  {
                    deleteDir()
                    env.PATH = "/usr/local/bin:${env.PATH}"
                    
                    int ntry = 5
                    while (ntry != 0)
                    {
                      try {
                        checkout scm
                        ntry = 0
                      }
                      catch (IOException e)
                      {
                        ntry--
                        sleep(10)
                      }
                    }

                    stage ('build_sb15_val')
                    {
                      sh "echo $PATH && ./build.sh $WORKSPACE $NODE_NAME VALGRIND $BRANCH_NAME"
                    }

                    stage ('run_sb15_val')
                    {
                      sh "cd openfpm_data && ./run.sh $WORKSPACE $NODE_NAME VALGRIND"
                      sh "cd openfpm_data && ./success.sh 1 sbalzarini-mac-15 openfpm_data valgrind"
                    }
                  }
                 },


"sb15_NO" : {node ('sbalzarini-mac-15')
                  {
                    deleteDir()
                    env.PATH = "/usr/local/bin:${env.PATH}"

                    int ntry = 5
                    while (ntry != 0)
                    {
                      try {
                        checkout scm
                        ntry = 0
                      }
                      catch (IOException e)
                      {
                        ntry--
                        sleep(10)
                      }
                    }

                    stage ('build_sb15_nor')
                    {
                      sh "./build.sh $WORKSPACE $NODE_NAME NO $BRANCH_NAME"
                    }

                    stage ('run_sb15_nor')
                    {
                      sh "cd openfpm_data && ./run.sh $WORKSPACE $NODE_NAME NO"
                      sh "cd openfpm_data && ./success.sh 2 sbalzarini-mac-15 openfpm_data"
                    }
                  }
                 },

"sb15_SE" : {node ('sbalzarini-mac-15')
                  {
                    deleteDir()
                    env.PATH = "/usr/local/bin:${env.PATH}"
                    
                    int ntry = 5
                    while (ntry != 0)
                    {
                      try {
                        checkout scm
                        ntry = 0
                      }
                      catch (IOException e)
                      {
                        ntry--
                        sleep(10)
                      }
                    }

                    stage ('build_sb15_se')
                    {
                      sh "./build.sh $WORKSPACE $NODE_NAME SE $BRANCH_NAME"
                    }

                    stage ('run_sb15_se')
                    {
                      sh "cd openfpm_data && ./run.sh $WORKSPACE $NODE_NAME SE"
                      sh "cd openfpm_data && ./success.sh 1 sbalzarini-mac-15 openfpm_data \"security enhancements\""
                    }
                  }
                 }



)


def protectedBranch = 'main'

pipeline {
    agent any
    stages {
        stage('Clone') {
            steps {
                script {
                    git(
                        url: scm.userRemoteConfigs[0].url,
                        branch: env.BRANCH_NAME,
                        credentialsId: 'github-token'
                    )
                }
            }
        }
        stage('Configure') {
            steps {
                script {
                    def presets = bat(script: 'cmake --list-presets', returnStdout: true)
                        .split('Available configure presets:')[1]
                        .trim()
                        .tokenize('\n')
                        .collect { it.trim().replace('"', '') }
                    for (preset in presets) {
                        def script = [
                            'cmake',
                            '--preset',
                            '"' + preset + '"'
                        ].join(' ')
                        bat(script: script)
                    }
                }
            }
        }
        stage('Build') {
            steps {
                script {
                    def presets = bat(script: 'cmake --list-presets', returnStdout: true)
                        .split('Available configure presets:')[1]
                        .trim()
                        .tokenize('\n')
                        .collect { it.trim().replace('"', '') }
                    for (preset in presets) {
                        def script = [
                            'cmake',
                            '--build',
                            '--preset',
                            '"' + preset + '"',
                            '--target',
                            'cmake-install'
                        ].join(' ')
                        bat(script: script)
                    }
                }
            }
        }
        stage('Export') {
            when {
                branch protectedBranch
            }
            steps {
                script {
                    def presets = bat(script: 'cmake --list-presets', returnStdout: true)
                        .split('Available configure presets:')[1]
                        .trim()
                        .tokenize('\n')
                        .collect { it.trim().replace('"', '') }
                    for (preset in presets) {
                        def script = [
                            'cmake',
                            '--build',
                            '--preset',
                            '"' + preset + '"',
                            '--target',
                            'conan-export'
                        ].join(' ')
                        bat(script: script)
                    }
                }
            }
        }
    }
    post {
        cleanup {
            cleanWs()
        }
    }
}

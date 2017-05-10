pipeline {
	agent any

	stages {
		stage('Build') {
			steps {
				sh 'cmake .'
				sh 'make libRCON_static libRCON_shared'
				archiveArtifacts artifacts: 'libRCON.a'
				archiveArtifacts artifacts: 'libRCON.so'
			}
		}
	}
}

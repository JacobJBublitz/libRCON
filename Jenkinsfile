pipeline {
	agent any

	stages {
		stage('Build') {
			steps {
				sh 'cmake .'
				sh 'make'
				archiveArtifacts artifacts: 'libRCON.a'
			}
		}
	}
}

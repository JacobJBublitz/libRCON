pipeline {
	agent any

	stages {
		stage('Build') {
			sh 'cmake .'
			sh 'make'
			archiveArtifacts artifacts: 'libRCON.a'
		}
	}
}

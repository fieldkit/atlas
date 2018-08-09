@Library('conservify') _

conservifyProperties()

timestamps {
    node () {
        conservifyBuild(name: 'atlas')

        build job: "distribution", wait: false
    }
}

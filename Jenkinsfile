@Library('conservify') _

conservifyProperties()

timestamps {
    node () {
        conservifyBuild(name: 'atlas')
        distributeFirmware()
    }

    refreshDistribution()
}

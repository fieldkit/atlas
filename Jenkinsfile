@Library('conservify') _

conservifyProperties()

timestamps {
    node () {
        conservifyBuild(name: 'atlas', archive: true)
        distributeFirmware()
    }

    refreshDistribution()
}

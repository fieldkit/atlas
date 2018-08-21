@Library('conservify') _

conservifyProperties()

timestamps {
    node () {
        conservifyBuild(name: 'atlas', archive: "build/firmware/module/*.bin")
        distributeFirmware()
    }

    refreshDistribution()
}

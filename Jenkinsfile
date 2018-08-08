@Library('conservify') _

timestamps {
    node () {
        conservifyBuild(name: 'atlas', repository: 'https://github.com/fieldkit/atlas.git')

        build job: "distribution", parameters: []
    }
}

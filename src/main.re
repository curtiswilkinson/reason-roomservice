let options = Args.parse(Sys.argv);

let config = Config.get(options.project);

Js.log(options);
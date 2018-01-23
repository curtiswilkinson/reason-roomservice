let options = Args.parse(Sys.argv);

let config = Config.get(options.project);

Cache.parse(config) |> Js.log;

Js.log(options);
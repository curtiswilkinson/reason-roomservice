let options = Args.parse(Sys.argv);

let config = Config.get(options.project);

let handle_ignore: (list(string), Config.room) => Config.room =
  (ignored, room) => {
    let isIgnored = List.exists(x => x == room##name, ignored);
    if (isIgnored) {
      Js.Obj.assign(room, {"shouldBuild": false});
    } else {
      room;
    };
  };

let transform: (Args.Options.t, Config.t) => Config.t =
  (options, config) =>
    if (List.length(options.ignore) > 0) {
      Js.Obj.assign(
        config,
        {"rooms": Array.map(handle_ignore(options.ignore), config##rooms)}
      );
    } else {
      config;
    };

Cache.parse(config) |> transform(options) |> Js.log;
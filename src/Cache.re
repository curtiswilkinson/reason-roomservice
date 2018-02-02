[@bs.val] external home_dir : string = "process.env.HOME";

[@bs.module] external md5 : string => string = "md5";

let base_path = home_dir ++ "/roomservice/";

let gen_cache_path = path => base_path ++ md5(path);

let process_room: Config.room => Config.room =
  room => {
    open Node;
    let cachePath = gen_cache_path(room##path);
    if (! Fs.existsSync(cachePath)) {
      Js.Obj.assign(room, {"shouldBuild": true});
    } else {
      let cmd = "find " ++ room##path ++ " -cnewer " ++ cachePath;
      switch (
        ChildProcess.execSync(cmd, Node.Options.options()) |> String.length
      ) {
      | 0 => Js.Obj.assign(room, {"shouldBuild": false})
      | _ => Js.Obj.assign(room, {"shouldBuild": true})
      };
    };
  };

let parse: Config.t => Config.t =
  conf => Js.Obj.assign(conf, {"rooms": Array.map(process_room, conf##rooms)});
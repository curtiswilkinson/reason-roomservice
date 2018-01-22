let options = Args.parse(Sys.argv);

Config.get("./mock/roomservice.config.yml");

Js.log(options);
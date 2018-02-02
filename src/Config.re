type hook =
  | Single(option(string))
  | Batch(option(list(string)));

type room = {
  .
  "name": string,
  "path": string,
  "before": hook,
  "runParallel": hook,
  "runSynchronous": hook,
  "after": hook,
  "finally": hook,
  "cached": bool,
  "shouldBuild": bool
};

type t = {. "rooms": array(room)};

let is_file: string => bool = [%raw
  {|
  function(path) {
    return Fs.lstatSync(path).isFile()
  }
|}
];

let parse_json: string => 'a = [%raw
  {|
    function(content) {
      return JSON.parse(content)
    }
  |}
];

/* the greatest lie */
let extend_config: t => t = [%raw
  {|
    function(rawConfig) {
      var rooms = Object.keys(rawConfig.rooms).reduce((acc, current) => {
        var room = rawConfig.rooms[current]
        room.name = current
        room.cached = false
        room.shouldBuild = true
        acc.push(room)
        return acc
      }, [])
      return { rooms }
    }
  |}
];

[@bs.module "js-yaml"] external parse_yaml : string => 'a = "load";

[@bs.module "toml"] external parse_toml : string => 'a = "parse";

let normalise_config_paths = (rootPath, config) => {
  Js.log(config##rooms);
  let resolve_room_path = room =>
    Js.Obj.assign(room, {"path": Node.Path.join2(rootPath, room##path)});
  Js.Obj.assign(
    config,
    {"rooms": Array.map(resolve_room_path, config##rooms)}
  );
};

let rec first_matching_config = files =>
  switch files {
  | [] =>
    Js.log("No files in dir");
    exit(1);
  | ["roomservice.config.yml"] => "roomservice.config.yml"
  | ["roomservice.config.yaml"] => "roomservice.config.yaml"
  | ["roomservice.config.toml"] => "roomservice.config.toml"
  | ["roomservice.config.json"] => "roomservice.config.json"
  | [_, ...xs] => first_matching_config(xs)
  };

let parse = (path, contents) => {
  open Node;
  let ext = Path.parse(path)##ext;
  switch ext {
  | ".yml" => parse_yaml(contents)
  | ".yaml" => parse_yaml(contents)
  | ".toml" => parse_toml(contents)
  | ".json" => parse_json(contents)
  | _ =>
    Js.log("unknown config format");
    exit(1);
  };
};

let get = projectPath => {
  open Node;
  let isFile = is_file(projectPath);
  let path =
    if (isFile) {
      projectPath;
    } else {
      Fs.readdirSync(projectPath)
      |> Array.to_list
      |> first_matching_config
      |> Path.join2(projectPath);
    };
  let contents = Fs.readFileAsUtf8Sync(path);
  let rootPath = isFile ? Path.dirname(projectPath) : projectPath;
  parse(path, contents) |> extend_config |> normalise_config_paths(rootPath);
};
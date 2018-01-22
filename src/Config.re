type hook =
  | Single(option(string))
  | Batch(option(list(string)));

type room = {
  path: option(string),
  before: hook,
  runParallel: hook,
  runSynchronous: hook,
  after: hook,
  finally: hook
};

type config = {rooms: list(room)};

let is_file: string => bool = [%raw
  {|
  function(path) {
    return Fs.lstatSync(path).isFile()
  }
|}
];

let parse_json: string => config = [%raw
  {|
    function(content) {
      return JSON.parse(content)
    }
  |}
];

[@bs.module "js-yaml"] external parse_yaml : string => config = "load";

[@bs.module "toml"] external parse_toml : string => config = "parse";

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
  let config = parse(path, contents);
  Js.log(config);
  config;
};
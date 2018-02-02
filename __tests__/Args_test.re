open Jest;

describe("Args", () =>
  Expect.(
    test("consume", () =>
      expect(Args.consume([||], Args.Options.default, "--init").init)
      |> toBe(true)
    )
  )
);
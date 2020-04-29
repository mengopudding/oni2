open Oni_Core;
open Oni_Model;
open Oni_IntegrationTestLib;
open EditorCoreTypes;

// Validate that unsaved changes persist when switching buffers
// Regression test for: https://github.com/onivim/oni2/issues/1671
runTest(~name="Regression1671 - Opening new buffer loses changes in previous buffer", (dispatch, wait, runEffects) => {
  wait(~name="Capture initial state", (state: State.t) =>
    state.vimMode == Vim.Types.Normal
  );

  Vim.command("e a-test-file");
  Vim.input("iabc");

  // Wait for lines to be available in buffer
  let originalBufferId = ref(-1);
  wait(~name="Buffer gets updated", (state: State.t) =>
    state
    |> Selectors.getActiveBuffer
    |> Option.map(buffer => {
      let lines = Buffer.getLines(buffer);
      originalBufferId := Buffer.getId(buffer);
      prerr_endline (lines[0]);
      lines[0] == "abc"
    })
    |> Option.value(~default=false)
  );
  
  // Repro for 1671 - switch to a different file, and then back
  Vim.input("<ESC>");
  Vim.command("e! another-test-file");
  wait(~name="Buffer switched", (state: State.t) => {
    state
    |> Selectors.getActiveBuffer
    |> Option.map(buffer => {
      Buffer.getId(buffer) != originalBufferId^
      })
    |> Option.value(~default=false);
  });

  Vim.command("e a-test-file");
  
  // Buffer modifications should still be around!
  wait(~name="Buffer updates are still around after switching back", (state: State.t) =>
    state
    |> Selectors.getActiveBuffer
    |> Option.map(buffer => {
      let lines = Buffer.getLines(buffer);
      prerr_endline (lines[0]);
      lines[0] == "abc"
    })
    |> Option.value(~default=false)
  );
});

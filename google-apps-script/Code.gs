function doGet(e) {
  try {
    // Check if parameters exist
    if (!e || !e.parameter) {
      Logger.log("No parameters - script was run from editor, not via URL");
      return ContentService.createTextOutput("ERROR: No parameters received");
    }

    var spreadsheet = SpreadsheetApp.getActiveSpreadsheet();

    // USE "Sheet1" - that's your actual sheet name!
    var sheet = spreadsheet.getSheetByName("Sheet1");

    if (!sheet) {
      Logger.log("Sheet1 not found!");
      return ContentService.createTextOutput("ERROR: Sheet1 not found");
    }

    // Get parameters
    var roll = e.parameter.roll || "";
    var name = e.parameter.name || "";
    var classSection = e.parameter.classSection || "";
    var date = e.parameter.date || "";
    var time = e.parameter.time || "";

    // Log what we received
    Logger.log(
      "Received - Roll: " +
        roll +
        ", Name: " +
        name +
        ", Class: " +
        classSection +
        ", Date: " +
        date +
        ", Time: " +
        time
    );

    // Add to sheet
    sheet.appendRow([roll, name, classSection, date, time]);

    Logger.log("✓ Row added successfully");
    return ContentService.createTextOutput("OK");
  } catch (error) {
    Logger.log("ERROR: " + error.toString());
    return ContentService.createTextOutput("ERROR: " + error.toString());
  }
}

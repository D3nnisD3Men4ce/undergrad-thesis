function doGet(e) { 
  Logger.log( JSON.stringify(e) );  // view parameters
  var result = 'Ok'; // assume success
  if (e.parameter == 'undefined') {
    result = 'No Parameters';
  }
  else {
    var sheet_id = ''; 		// Spreadsheet ID
    var sheet_name = "plantData";
    var sheet = SpreadsheetApp.openById(sheet_id).getSheetByName(sheet_name);		// get Active sheet
    var newRow = sheet.getLastRow() + 1;						
    var rowData = [];
    d=new Date();
    rowData[0] = d; // Timestamp in column A
    rowData[1] = d.toLocaleTimeString(); // Timestamp in column A
    
    for (var param in e.parameter) {
      Logger.log('In for loop, param=' + param);
      var value = stripQuotes(e.parameter[param]);
      Logger.log(param + ':' + e.parameter[param]);
      switch (param) {
        case 'ec': //Parameter 1, It has to be updated in Column in Sheets in the code, orderwise
          rowData[2] = value;
          result = 'Written on column ec';
          break;
        case 'ph': //Parameter 2, It has to be updated in Column in Sheets in the code, orderwise
          rowData[3] = value;
          result += ' Written on column ph';
          break;
        case 'windSpeed': //Parameter 3, It has to be updated in Column in Sheets in the code, orderwise
          rowData[4] = value;
          result += ' Written on column windSpeed';
          break;
        case 'ambientTemp': //Parameter 3, It has to be updated in Column in Sheets in the code, orderwise
          rowData[5] = value;
          result += ' Written on column ambientTemp';
          break;
        case 'waterTemp': //Parameter 3, It has to be updated in Column in Sheets in the code, orderwise
          rowData[6] = value;
          result += ' Written on column waterTemp';
          break;
        case 'hum': //Parameter 3, It has to be updated in Column in Sheets in the code, orderwise
          rowData[7] = value;
          result += ' Written on column hum';
          break;
        case 'atmosPressure': //Parameter 3, It has to be updated in Column in Sheets in the code, orderwise
          rowData[8] = value;
          result += ' Written on column atmosPressure';
          break;
        default:
          result = "unsupported parameter";
      }
    }
    Logger.log(JSON.stringify(rowData));
    // Write new row below
    var newRange = sheet.getRange(newRow, 1, 1, rowData.length);
    newRange.setValues([rowData]);
  }
  // Return result of operation
  return ContentService.createTextOutput(result);
}
function stripQuotes( value ) {
  return value.replace(/^["']|['"]$/g, "");
}
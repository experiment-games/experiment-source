local PANEL = {}

-- Adds a new object in JavaScript
function PANEL:NewObject(objectName)
	self:AddJavascriptObject(objectName)
end

-- Adds a new object callback in JavaScript
function PANEL:NewObjectCallback(objectName, callbackName)
	self:AddJavascriptObjectCallback(objectName, callbackName)
end

-- TODO: Adds javascript function 'language.Update' to an HTML panel as a method to call Lua's language.GetPhrase function.
function JS_Language(htmlPanel)
end

-- TODO: Adds javascript function 'util.MotionSensorAvailable' to an HTML panel as a method to call Lua's motionsensor.IsAvailable function.
function JS_Utility(htmlPanel)
end

-- TODO: Adds workshop related javascript functions to an HTML panel, used by the "Dupes" and "Saves" tabs in the spawnmenu.
function JS_Workshop(htmlPanel)
end

derma.DefineControl("Awesomium", "A wrapper around the HTML to simulate awesomium", PANEL, "HTML")

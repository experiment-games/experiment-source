local CImagePanel = {}

function CImagePanel:Init()
	self:SetMouseInputEnabled(false)
	self:SetKeyboardInputEnabled(false)

	self.ImageName = ""

	self.ActualWidth = 16
	self.ActualHeight = 16
end

function CImagePanel:SetImage(strImage)
	self.ImageName = strImage

	if (not self.TextureID) then
		self.TextureID = Surface.CreateNewTextureID(true)
	end

	self.Material = Surface.FindMaterial(strImage)

	local texture = self.Material:GetTexture("$basetexture")

	self.ActualWidth = texture:GetActualWidth()
	self.ActualHeight = texture:GetActualHeight()
end

function CImagePanel:GetImage()
	return self.ImageName
end

function CImagePanel:SizeToContents()
	self:SetSize(self.ActualWidth, self.ActualHeight)
end

function CImagePanel:Paint()
	local width, height = self:GetSize()

	if (! self.Material) then
		return
	end

	Surface.DrawSetTextureMaterial(self.TextureID, self.Material)
	Surface.DrawSetColor(255, 255, 255, 255)
	Surface.DrawTexturedRect(0, 0, width, height)
end

Gui.Register(CImagePanel, "CImagePanel", "Panel")

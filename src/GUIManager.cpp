// This file is part of Revolution.
// 
// Revolution is free software: you can redistribute it and/or modify it under 
// the terms of the GNU General Public License as published by the 
// Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.
// 
// Revolution is distributed in the hope that it will be useful, but WITHOUT 
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for 
// more details.
// 
// You should have received a copy of the GNU General Public License along with 
// Revolution. If not, see <http://www.gnu.org/licenses/>.
#include <iostream>
#include <GL/glew.h>
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/OpenGL/CEGUIOpenGLRenderer.h>
#include "GUIManager.hpp"

GUIManager::GUIManager ()
{
  CEGUIInstallBasePath = "/home/neal/shared/college/hci/code/";
  init();
}

GUIManager::~GUIManager ()
{
}

void GUIManager::init ()
{
  std::cout << " - initializing CEGUI" << std::endl;
  CEGUI::OpenGLRenderer::bootstrapSystem();

  setCEGUIPaths();
  CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
  CEGUI::System::getSingleton().setDefaultFont("DejaVuSans-10");
  CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook",
      "MouseArrow");
  createGUI();
}

void GUIManager::render ()
{
  glUseProgram(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  // Renders the GUI:
  CEGUI::System::getSingleton().renderGUI();
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
}

void GUIManager::setCEGUIPaths ()
{
	// Initialises the required directories for the DefaultResourceProvider:
	CEGUI::DefaultResourceProvider & defaultResProvider =
    *static_cast<CEGUI::DefaultResourceProvider*>(
        CEGUI::System::getSingleton().getResourceProvider());
	const std::string CEGUIInstallSharePath = CEGUIInstallBasePath
	  + "share/";
	// For each resource type, sets a corresponding resource group directory:
	std::cout << "Using scheme directory '" << CEGUIInstallSharePath + "schemes/"
		 << "'" << std::endl;
	defaultResProvider.setResourceGroupDirectory("schemes",
	  CEGUIInstallSharePath + "schemes/");
	defaultResProvider.setResourceGroupDirectory("imagesets",
	  CEGUIInstallSharePath + "imagesets/");
	defaultResProvider.setResourceGroupDirectory("fonts",
	  CEGUIInstallSharePath + "fonts/");
	defaultResProvider.setResourceGroupDirectory("layouts",
	  CEGUIInstallSharePath + "layouts/");
	defaultResProvider.setResourceGroupDirectory("looknfeels",
	  CEGUIInstallSharePath + "looknfeel/");
	defaultResProvider.setResourceGroupDirectory("lua_scripts",
	  CEGUIInstallSharePath + "lua_scripts/");
	defaultResProvider.setResourceGroupDirectory("schemas",
	  CEGUIInstallSharePath + "xml_schemas/");
	defaultResProvider.setResourceGroupDirectory("animations",
	  CEGUIInstallSharePath + "animations/");
	// Sets the default resource groups to be used:
	CEGUI::Imageset::setDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
	CEGUI::WindowManager::setDefaultResourceGroup( "layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
	CEGUI::AnimationManager::setDefaultResourceGroup("animations");
 
	// Set-up default group for validation schemas:
	CEGUI::XMLParser *parser = CEGUI::System::getSingleton().getXMLParser();
	if ( parser->isPropertyPresent("SchemaDefaultResourceGroup"))
		parser->setProperty("SchemaDefaultResourceGroup", "schemas");
}

void GUIManager::createGUI ()
{
  std::cout << "Creating the GUI..." << std::endl;
  try {
    CEGUI::WindowManager & wm = CEGUI::WindowManager::getSingleton();
    CEGUI::System & sys = CEGUI::System::getSingleton();
    CEGUI::Window *root = wm.createWindow("DefaultWindow", "_MasterRoot");
    sys.setGUISheet(root);
    CEGUI::Window *sheet = wm.loadWindowLayout("TaharezRevolution.layout");
    sys.getGUISheet()->addChildWindow(sheet);
  }
#ifndef NOCEGUI
  catch (CEGUI::Exception & e) {
    std::cerr << "CEGUI Exception: " << e.getMessage() << std::endl;
    // TODO throw custom exception to pass to screen.
  }
#endif
}

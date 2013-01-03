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
    setHandlers();
  }
  catch (CEGUI::Exception & e ) {
    std::cerr << "CEGUI Exception: " << e.getMessage() << std::endl;
    throw(e.getMessage());
  }
}

void GUIManager::populateDatalists ( std::vector<DataManager *> const & list )
{
  CEGUI::WindowManager & wm = CEGUI::WindowManager::getSingleton();
  auto populate = [&]( std::string const & name,
      DataManagerType const & type ) {
    CEGUI::Listbox *lb = static_cast<CEGUI::Listbox *>(wm.getWindow(
          "Sheet/DatasetFrame/TabControl/"+name+"/Listbox"));
    unsigned int i = 0;
    for (DataManager *p : list) {
      if (p != NULL && p->type == type) {
        ListboxItem *item = new ListboxItem(p->name, i);
        item->setUserData(p);
        lb->addItem(item);
      }
      i++;
    }
  };
  populate("HTab", DM_Height);
  populate("PTab", DM_Pattern);
  populate("CTab", DM_Color);
}

void GUIManager::setHandlers ()
{
  CEGUI::WindowManager & wm = CEGUI::WindowManager::getSingleton();

  CEGUI::FrameWindow *dsFrame = static_cast<CEGUI::FrameWindow *>(
      wm.getWindow("Sheet/DatasetFrame"));
  dsFrame->hide();
  // Handle behavior of options-button
  CEGUI::PushButton *button = static_cast<CEGUI::PushButton *>(
      wm.getWindow("Sheet/Options"));
  button->subscribeEvent(CEGUI::PushButton::EventClicked,
      CEGUI::Event::Subscriber(&GUIManager::handleOptionsVisibility, this));
  button = dsFrame->getCloseButton();
  button->subscribeEvent(CEGUI::PushButton::EventClicked,
      CEGUI::Event::Subscriber(&GUIManager::handleOptionsVisibility, this));
  // Connect activate buttons on tabs
  auto connectFrames = [=](CEGUI::Window *tab) {
    CEGUI::PushButton *button =
      static_cast<CEGUI::PushButton *>(tab->getChild(3));
    button->subscribeEvent(CEGUI::PushButton::EventClicked,
        CEGUI::Event::Subscriber(&GUIManager::handleDSActivation, this));
    CEGUI::Scrollbar *sb = static_cast<CEGUI::Scrollbar *>(tab->getChild(2));
    sb->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged,
        CEGUI::Event::Subscriber(&GUIManager::handleScrollbarChanged, this));
    CEGUI::Listbox *lb = static_cast<CEGUI::Listbox *>(tab->getChild(0));
    lb->subscribeEvent(CEGUI::Listbox::EventSelectionChanged,
        CEGUI::Event::Subscriber(&GUIManager::handleDSSelection, this));
  };
  CEGUI::Window *tab =  wm.getWindow("Sheet/DatasetFrame/TabControl/HTab");
  connectFrames(tab);
  tab = wm.getWindow("Sheet/DatasetFrame/TabControl/PTab");
  connectFrames(tab);
  tab = wm.getWindow("Sheet/DatasetFrame/TabControl/CTab");
  connectFrames(tab);
  CEGUI::Scrollbar *sb = static_cast<CEGUI::Scrollbar *>(
      wm.getWindow("Sheet/DimensionSlider"));
  sb->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged,
      CEGUI::Event::Subscriber(&GUIManager::handleBigScrollbarChanged, this));
}

bool GUIManager::handleOptionsVisibility ( CEGUI::EventArgs const & )
{
  CEGUI::Window *dsFrame = CEGUI::WindowManager::getSingleton().getWindow(
      "Sheet/DatasetFrame");
  std::string prop = dsFrame->getProperty("Visible").c_str();
  if (prop == "False")
    dsFrame->show();
  else
    dsFrame->hide();
  return true;
}

bool GUIManager::handleDSActivation ( CEGUI::EventArgs const & e )
{
  CEGUI::Window *tab =
    static_cast<CEGUI::WindowEventArgs const &>(e).window->getParent();
  CEGUI::Listbox *lb = static_cast<CEGUI::Listbox *>(tab->getChild(0));
  ListboxItem *item = static_cast<ListboxItem *>(lb->getFirstSelectedItem());
  if (item != NULL) {
    DataManager *dm = static_cast<DataManager *>(item->getUserData());
    CEGUI::Scrollbar *sb = static_cast<CEGUI::Scrollbar *>(tab->getChild(2));
    std::vector<unsigned int> const & dims = dm->getDimensions();
    unsigned int dim = dims[int(sb->getScrollPosition()*(dims.size()-1))];
    float scrollPos = sb->getScrollPosition();
    dm->activate(dim);
    // Enable global scrollbar
    CEGUI::WindowManager & wm = CEGUI::WindowManager::getSingleton();
    sb = static_cast<CEGUI::Scrollbar *>(wm.getWindow("Sheet/DimensionSlider"));
    sb->enable();
    CEGUI::WindowEventArgs w(sb);
    sb->fireEvent(CEGUI::Scrollbar::EventScrollPositionChanged, w);
    // Set the global scrollbar to the right position.
    sb->setScrollPosition(scrollPos);
    CEGUI::Window *desc = wm.getWindow("Sheet/DimensionText");
    desc->show();
  }
  // TODO handle else-error
  return true;
}

bool GUIManager::handleDSSelection ( CEGUI::EventArgs const & e )
{
  CEGUI::Window *tab =
    static_cast<CEGUI::WindowEventArgs const &>(e).window->getParent();
  CEGUI::Listbox *lb = static_cast<CEGUI::Listbox *>(tab->getChild(0));
  CEGUI::Scrollbar *sb = static_cast<CEGUI::Scrollbar *>(tab->getChild(2));
  DataManager *dm = static_cast<DataManager *>(
      lb->getFirstSelectedItem()->getUserData());
  _selectedDM = dm;
  std::vector<unsigned int> const & dim = dm->getDimensions();
  sb->setStepSize(1.0/float(dim.size()-1));
  sb->enable();
  CEGUI::WindowEventArgs w(sb);
  sb->fireEvent(CEGUI::Scrollbar::EventScrollPositionChanged, w);
  return true;
}

bool GUIManager::handleScrollbarChanged ( CEGUI::EventArgs const & e )
{
  CEGUI::Window *tab =
    static_cast<CEGUI::WindowEventArgs const &>(e).window->getParent();
  CEGUI::Scrollbar *scrollbar =
    static_cast<CEGUI::Scrollbar *>(tab->getChild(2));
  CEGUI::Listbox *lb = static_cast<CEGUI::Listbox *>(tab->getChild(0));
  CEGUI::Window *desc = tab->getChild(1);
  DataManager *dm = static_cast<DataManager *>(
      lb->getFirstSelectedItem()->getUserData());
  std::vector<unsigned int> const & dims = dm->getDimensions();
  float f = static_cast<CEGUI::Scrollbar *>(scrollbar)->getScrollPosition();
  unsigned int dim = dims[int(f*(dims.size()-1))];
  std::ostringstream ss; ss << "Dimension: " << dim;
  desc->setText(ss.str());
  return true;
}

bool GUIManager::handleBigScrollbarChanged ( CEGUI::EventArgs const & e )
{
  CEGUI::WindowManager & wm = CEGUI::WindowManager::getSingleton();
  try {
    if (_selectedDM != NULL) {
      CEGUI::Window *desc = wm.getWindow("Sheet/DimensionText");
      CEGUI::Scrollbar *sb = static_cast<CEGUI::Scrollbar *>(
          static_cast<CEGUI::WindowEventArgs const &>(e).window);
      float f = sb->getScrollPosition();
      std::vector<unsigned int> const & dims = _selectedDM->getDimensions();
      unsigned int newDim = dims[int(f*(dims.size()-1))];
      std::ostringstream ss; ss << newDim;
      desc->setText(ss.str());
      CEGUI::Scrollbar *otherSB;
      switch (_selectedDM->type) {
        case DM_Height:
          otherSB = static_cast<CEGUI::Scrollbar *>(wm.getWindow(
                "Sheet/DatasetFrame/TabControl/HTab/Scrollbar"));
          break;
        case DM_Pattern:
          otherSB = static_cast<CEGUI::Scrollbar *>(wm.getWindow(
                "Sheet/DatasetFrame/TabControl/PTab/Scrollbar"));
          break;
        case DM_Color:
          otherSB = static_cast<CEGUI::Scrollbar *>(wm.getWindow(
                "Sheet/DatasetFrame/TabControl/CTab/Scrollbar"));
          break;
        default:
          break;
      };
      otherSB->setScrollPosition(f);
      _selectedDM->activate(newDim);
    }
  }
  catch (DataManagerException & e) {
    std::cerr << e.what() << std::endl;
  }
  return true;
}

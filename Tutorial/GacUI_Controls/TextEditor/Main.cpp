#define GAC_HEADER_USE_NAMESPACE
#include "UI/Source/Demo.h"
#include <Windows.h>

using namespace vl::collections;
using namespace vl::stream;

#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	return SetupWindowsDirect2DRenderer();
}

void GuiMain()
{
	{
		List<WString> errors;
		FileStream fileStream(L"../UIRes/TextEditor.bin", FileStream::ReadOnly);
		auto resource = GuiResource::LoadPrecompiledBinary(fileStream, errors);
		GetResourceManager()->SetResource(L"Resource", resource);
	}
	demo::MainWindow window;
	window.MoveToScreenCenter();
	GetApplication()->Run(&window);
}

/***********************************************************************
OpenUrl
***********************************************************************/

void OpenUrl(const WString& url)
{
	ShellExecute(NULL, L"OPEN", url.Buffer(), NULL, NULL, SW_MAXIMIZE);
}

/***********************************************************************
SetTokenizer
***********************************************************************/

class XmlColorizer : public GuiTextBoxRegexColorizer
{
public:
	XmlColorizer()
	{
		text::ColorEntry entry = GetCurrentTheme()->GetDefaultTextBoxColorEntry();
		SetDefaultColor(entry);

		entry.normal.text = Color(0, 128, 0);
		AddToken(L"/<!--([^/-]|-[^/-]|--[^>])*--/>", entry);

		entry.normal.text = Color(128, 0, 255);
		AddToken(L"/<!/[CDATA/[([^/]]|/][^/]]|/]/][^>])*/]/]/>", entry);

		entry.normal.text = Color(0, 0, 0);
		AddToken(L"\"[^\"]*\"", entry);

		entry.normal.text = Color(0, 0, 255);
		AddToken(L"[<>=]", entry);

		entry.normal.text = Color(255, 0, 0);
		AddToken(L"[a-zA-Z0-9_/-:]+", entry);

		entry.normal.text = Color(163, 21, 21);
		AddExtraToken(entry);

		Setup();
	}

	void ColorizeTokenContextSensitive(vint lineIndex, const wchar_t* text, vint start, vint length, vint& token, vint& contextState)override
	{
		// 0 < 1 name 2 att > 0
		switch (token)
		{
		case 3:
			if (length == 1)
			{
				switch (text[start])
				{
				case '<':
					contextState = 1;
					break;
				case '>':
					contextState = 0;
					break;
				}
			}
			break;
		case 4:
			switch (contextState)
			{
			case 0:
				token = -1;
				break;
			case 1:
				token = 5;
				contextState = 2;
				break;
			}
			break;
		}
	}

	vint GetContextStartState()override
	{
		return 0;
	}
};

void SetColorizer(GuiMultilineTextBox* textBox, bool forXml)
{
	textBox->SetColorizer(forXml ? new XmlColorizer : nullptr);
}
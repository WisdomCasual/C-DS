#include "ImGui.h"                // https://github.com/ocornut/imgui
#include "imgui_markdown.h"       // https://github.com/juliettef/imgui_markdown
//#include "IconsFontAwesome5.h"    // https://github.com/juliettef/IconFontCppHeaders

#include <string>

void LinkCallback(ImGui::MarkdownLinkCallbackData data_);

ImGui::MarkdownImageData ImageCallback(ImGui::MarkdownLinkCallbackData data_);

void LinkCallback(ImGui::MarkdownLinkCallbackData data_);

void LoadFonts(float fontSize_);

void MarkdownFormatCallback(const ImGui::MarkdownFormatInfo& markdownFormatInfo_, bool start_);

void Markdown(const std::string& markdown_);

void clearCachedImages();
#include "Markdown.h"

// Following includes for Windows LinkCallback
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Shellapi.h"
#include "Image.h"

#include <unordered_map>


static ImGui::MarkdownConfig mdConfig;

static ImFont* H1 = NULL;
static ImFont* H2 = NULL;
static ImFont* H3 = NULL;
static ImFont* bold = NULL;
static ImFont* italic = NULL;

std::unordered_map<std::string, ImGui::MarkdownImageData> cached_images;

void LinkCallback(ImGui::MarkdownLinkCallbackData data_)
{
    std::string url(data_.link, data_.linkLength);
    if (!data_.isImage)
    {
        ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
}

ImGui::MarkdownImageData ImageCallback(ImGui::MarkdownLinkCallbackData data_)
{
    GLuint textureID;;
    int width, height;
    std::string path;

    for (int i = 0; i < data_.linkLength; i++)
        path.push_back(data_.link[i]);
    
    if (!cached_images.count(path)) {
        image_imp::loadImage(path.c_str(), &textureID, &width, &height);

        ImGui::MarkdownImageData imageDataResult;

        imageDataResult.isValid = true;
        imageDataResult.useLinkCallback = false;
        imageDataResult.user_texture_id = reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(textureID));;
        imageDataResult.original_size = ImVec2((float)width, (float)height);

        cached_images[path] = imageDataResult;
    }

    ImGui::MarkdownImageData& imageDataResult = cached_images[path];

    // For image resize when available size.x > image width, add
    ImVec2 const availableSize = ImGui::GetContentRegionAvail();
    if (imageDataResult.original_size.x > availableSize.x)
    {
        float const ratio = imageDataResult.original_size.y / imageDataResult.original_size.x;
        imageDataResult.size.x = availableSize.x;
        imageDataResult.size.y = availableSize.x * ratio;
    }
    else
    {
        imageDataResult.size = imageDataResult.original_size;
    }

    return imageDataResult;
}

void LoadFonts(float fontSize_)
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    // Base font
    ImFont* customFont = io.Fonts->AddFontFromFileTTF("Resources\\mainFont.ttf", fontSize_);

    // Set custom font as default
    io.FontDefault = customFont;

    // Bold headings H2 and H3
    H2 = H3 = io.Fonts->AddFontFromFileTTF("Resources\\mainFont-bold.ttf", fontSize_ * 1.1f);

    // bold heading H1
    H1 = io.Fonts->AddFontFromFileTTF("Resources\\mainFont-bold.ttf", fontSize_ * 1.2f);


    italic = io.Fonts->AddFontFromFileTTF("Resources\\mainFont-italic.ttf", fontSize_);
    bold = io.Fonts->AddFontFromFileTTF("Resources\\mainFont-bold.ttf", fontSize_);
}

void MarkdownFormatCallback(const ImGui::MarkdownFormatInfo& markdownFormatInfo_, bool start_)
{
    // Call the default first so any settings can be overwritten by our implementation.
    // Alternatively could be called or not called in a switch statement on a case by case basis.
    // See defaultMarkdownFormatCallback definition for furhter examples of how to use it.
    //ImGui::defaultMarkdownFormatCallback(markdownFormatInfo_, start_);

    switch (markdownFormatInfo_.type)
    {
    case ImGui::MarkdownFormatType::NORMAL_TEXT:
        break;
    case ImGui::MarkdownFormatType::EMPHASIS:
    {
        if (markdownFormatInfo_.level == 1)
        {
            // normal emphasis
            if (start_)
            {
                ImGui::PushFont(italic);
            }
            else
            {
                ImGui::PopFont();
            }
        }
        else
        {
            // strong emphasis
            if (start_)
            {
                ImGui::PushFont(bold);
            }
            else
            {
                ImGui::PopFont();
            }
        }
        break;
    }
    case ImGui::MarkdownFormatType::HEADING:
    {
        ImGui::MarkdownHeadingFormat fmt;
        if (markdownFormatInfo_.level > ImGui::MarkdownConfig::NUMHEADINGS)
        {
            fmt = markdownFormatInfo_.config->headingFormats[ImGui::MarkdownConfig::NUMHEADINGS - 1];
        }
        else
        {
            fmt = markdownFormatInfo_.config->headingFormats[markdownFormatInfo_.level - 1];
        }
        if (start_)
        {
            if (fmt.font)
            {
                ImGui::PushFont(fmt.font);
            }
            ImGui::NewLine();
        }
        else
        {
            if (fmt.separator)
            {
                ImGui::Separator();
                ImGui::NewLine();
            }
            else
            {
                ImGui::NewLine();
            }
            if (fmt.font)
            {
                ImGui::PopFont();
            }
        }
        break;
    }
    case ImGui::MarkdownFormatType::UNORDERED_LIST:
        break;
    case ImGui::MarkdownFormatType::LINK:
        if (start_)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
        }
        else
        {
            ImGui::PopStyleColor();
            if (markdownFormatInfo_.itemHovered)
            {
                SetCursor(::LoadCursor(NULL, IDC_HAND));
                ImGui::UnderLine(ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
            }
            else
            {
                ImGui::UnderLine(ImGui::GetStyle().Colors[ImGuiCol_Button]);
            }
        }
        break;

    default:
        break;
    }
}

void Markdown(const std::string& markdown_)
{
    // You can make your own Markdown function with your prefered string container and markdown config.
    // > C++14 can use ImGui::MarkdownConfig mdConfig{ LinkCallback, NULL, ImageCallback, ICON_FA_LINK, { { H1, true }, { H2, true }, { H3, false } }, NULL };
    mdConfig.linkCallback = LinkCallback;
    mdConfig.tooltipCallback = NULL;
    mdConfig.imageCallback = ImageCallback;
    //mdConfig.linkIcon = ICON_FA_LINK;
    mdConfig.headingFormats[0] = { H1, true };
    mdConfig.headingFormats[1] = { H2, true };
    mdConfig.headingFormats[2] = { H3, false };
    mdConfig.userData = NULL;
    mdConfig.formatCallback = MarkdownFormatCallback;
    ImGui::Markdown(markdown_.c_str(), markdown_.length(), mdConfig);
}

void clearCachedImages()
{
    cached_images.clear();
}

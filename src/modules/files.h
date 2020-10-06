/**IT_START**/

type ImageResponse = struct {
    id: s32;
    url: string;
    pixels: buffer;
    width: s32;
    height: s32;
}
type TextResponse = struct {
    id: s32;
    url: string;
    text: string;
}
import files {
    func loadText(string, func(TextResponse, s32), s32) -> s32;
    func loadImage(string, func(ImageResponse)) -> s32;
}

/**IT_END**/

#include <map>
#include <set>
#include <string>

class FileLoader {
    std::set<int> openRequests;
    std::map<std::string, TextResponse*> textResponses;

    static void onTextLoad(TextResponse* response, int loaderPtr) {
        auto loader = (FileLoader*)loaderPtr;
        loader->textResponses[std::string(response->url)] = response;
        loader->openRequests.erase(response->id);
    }
public:
    void beginFetch(std::string const& url) {
        int id = files::loadText(url.c_str(), FileLoader::onTextLoad, (int)this);
        openRequests.insert(id);
    }
    std::string read(std::string const& url) {
        return textResponses[url]->text;
    }
    bool anyPending() {
        return openRequests.size() > 0;
    }
};

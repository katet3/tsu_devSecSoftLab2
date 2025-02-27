#include <iostream>
#include <string>
#include <memory>

class Image {
public:
    std::string image_name_;
    int image_width_;
    int image_height_;

    Image(const std::string& image_name, int image_width, int image_height)
        : image_name_(image_name), image_width_(image_width), image_height_(image_height) {}

    ~Image() {
        std::cout << "Deleting image: " << image_name_ << std::endl;
    }

    void PrintInfo() const {
        std::cout << "Image: " << image_name_ << ", Width: " << image_width_ << ", Height: " << image_height_ << std::endl;
    }

    std::string GetName() const { return image_name_; }

    void SetWidth(int new_width) { image_width_ = new_width; }
};

class FileSystemHandler {
public:
    static void SaveImage(const Image& an_image) {
        std::cout << "Saving image '" << an_image.GetName() << "' to the file system." << std::endl;
    }

    static int DeleteDuplicates() {
        std::cout << "Deleting duplicate images from the file system." << std::endl;
        return 5;
    }
};

class DatabaseHandler {
public:
    static Image SetAccountPicture(const Image& an_image, const std::string& account_name) {
        std::cout << "Setting image '" << an_image.GetName() << "' as account picture for account '" << account_name << "'." << std::endl;
        return an_image;
    }
};

class ImageProcessor {
public:
    static Image ResizeImage(const Image& an_image, int new_width, int new_height) {
        Image resized_image(an_image.GetName(), new_width, new_height);
        std::cout << "Resizing image '" << an_image.GetName() << "' to " << new_width << "x" << new_height << "." << std::endl;
        return resized_image;
    }

    static Image InvertColors(const Image& an_image) {
        std::cout << "Inverting colors for image '" << an_image.GetName() << "'." << std::endl;
        return an_image;
    }
};

class NetworkHandler {
public:
    static Image* DownloadImage(const std::string& image_url) {
        std::cout << "Downloading image from URL: " << image_url << "." << std::endl;
        Image* ptr_downloaded_image = new Image("downloaded_image", 800, 600);
        return ptr_downloaded_image;
    }
};

int main() {
    Image original_image("example.jpg", 1920, 1080);
    original_image.PrintInfo();

    FileSystemHandler::SaveImage(original_image);

    int deleted_count = FileSystemHandler::DeleteDuplicates();
    std::cout << "Deleted " << deleted_count << " duplicate images." << std::endl;

    std::string account_name = "user123";
    Image account_picture = DatabaseHandler::SetAccountPicture(original_image, account_name);

    Image resized_image = ImageProcessor::ResizeImage(original_image, 800, 600);
    resized_image.PrintInfo();

    Image inverted_image = ImageProcessor::InvertColors(resized_image);
    inverted_image.PrintInfo();

    Image* ptr_downloaded_image = NetworkHandler::DownloadImage("http://example.com/image.jpg");
    if (ptr_downloaded_image) {
        ptr_downloaded_image->PrintInfo();
        delete ptr_downloaded_image;
    }

    return 0;
}
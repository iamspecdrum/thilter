juce::File getLicenseFile() {
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("squwbs").getChildFile("license.lic");
}

void saveLicenseFile(const juce::String& key) {
    auto file = getLicenseFile();
    file.getParentDirectory().createDirectory();
    file.replaceWithText(key); // Better: Encrypt/Sign this data
}

bool isLicensed = false;
// Inside Processor Constructor:
auto file = getLicenseFile();
if (file.existsAsFile()) {
    auto key = file.loadFileAsString();
    if (verifyKey(key)) // Implement your key verification logic
        isLicensed = true;
}
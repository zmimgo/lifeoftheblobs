struct settingsTemplate{
    int maxBlobs;
    int blobInvincibility;
    bool allowBlobAging;
    int blobMaxAge;
    bool showBlobAmountMessage;
    int blobAmountMessageDelay;
    bool includeYPC;
    bool showMajorityMessage;
    int blobMajorityMessageDelay;
    int universalBlobSpeed;
    bool extraRandomness;
    bool mercyRule;
    int minFramesBeforeMercy;
    int maxMercy;
};

// Settings templates

settingsTemplate Classic{
    500,
    100,
    false,
    1200,
    true,
    100,
    false,
    true,
    100,
    5,
    false,
    false,
    0,
    0
};

settingsTemplate Infinite{
    500,
    100,
    false,
    1200,
    true,
    100,
    true,
    true,
    100,
    5,
    false,
    false,
    0,
    0
};

settingsTemplate Normal{
    500,
    100,
    true,
    10000,
    true,
    100,
    true,
    true,
    100,
    5,
    false,
    false,
    0,
    0
};

settingsTemplate HungerGames{
    500,
    100,
    true,
    1200,
    true,
    100,
    true,
    true,
    100,
    5,
    false,
    true,
    10800,
    5
};


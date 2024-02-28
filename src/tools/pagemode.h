    QPushButton button ("Page Mode");
    QObject::connect(&button, &QPushButton::clicked, [&](){
        static bool pagemode = true;
        if (pagemode) {
            board->enable();
        } else {
            board->disable();
        }
        pagemode = !pagemode;
    });

    QPushButton button ("Page Mode");
    button.setFixedSize(100,100);
    QObject::connect(&button, &QPushButton::clicked, [&](){
        static bool pagemode = true;
        if (pagemode) {
            board->enable();
        } else {
            board->disable();
        }
        pagemode = !pagemode;
    });
    QPushButton eraser ("eraser");
    QObject::connect(&eraser, &QPushButton::clicked, [&](){
         window->eraser =! window->eraser;
    });

#include "recipewindow.h"
#include "ui_recipewindow.h"

RecipeWindow::RecipeWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecipeWindow)
{
    ui->setupUi(this);
}

RecipeWindow::~RecipeWindow()
{
    delete ui;
}

void RecipeWindow::populateTable() {
    ui->recipeWidget->clear();
    for (auto& s : recipeNames) {
        ui->recipeWidget->addItem(s);
    }
}


#include "utils.h"

#include "ApplicationWindowTest.h"
#include "table/Table.h"
#include "table/TableStatistics.h"

#include <iostream>

TEST_F(ApplicationWindowTest, Table)
{
    QLocale::setDefault(QLocale::c());

    Table *empty_table = newTable(0, 0);

    EXPECT_TRUE(empty_table->noXColumn());
    EXPECT_TRUE(empty_table->noYColumn());
    EXPECT_EQ(0, empty_table->YColumns().size());
    EXPECT_EQ("1", empty_table->name());
    EXPECT_EQ(0, empty_table->numRows());
    EXPECT_EQ(0, empty_table->numCols());
    EXPECT_EQ(0.0, empty_table->cell(1, 1));

    currentFolder()->removeWindow(empty_table);

    Table *table = newTable(10, 5);
    EXPECT_EQ("2", table->name());

    EXPECT_EQ(10, table->numRows());
    EXPECT_EQ(10, table->rowCount());

    EXPECT_EQ(5, table->numCols());
    EXPECT_EQ(5, table->columnCount());

    EXPECT_EQ(4, table->YColumns().size());
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(table->name() + "_" + table->column(i + 1)->name(), table->YColumns()[i]);
    }

    table->selectColumn(4);
    EXPECT_EQ(1, table->selectedColsNumber());
    EXPECT_EQ(table->name() + "_" + table->column(4)->name(), table->selectedColumns()[0]);
    EXPECT_EQ(1, table->selectedYColumns().size());
    EXPECT_EQ(table->name() + "_" + table->column(4)->name(), table->selectedYColumns()[0]);
    EXPECT_EQ(1, table->drawableColumnSelection().size());
    EXPECT_EQ(table->name() + "_" + table->column(4)->name(), table->drawableColumnSelection()[0]);
    EXPECT_EQ(table->column(4)->name(), table->selectedYLabels().value(4));
    table->insertCol();
    EXPECT_EQ(6, table->numCols());
    table->removeCol();
    EXPECT_EQ(5, table->numCols());
    table->deselect();
    EXPECT_EQ(0, table->selectedColsNumber());

    table->selectRow(9);
    EXPECT_EQ(1, table->numSelectedRows());
    table->insertRow();
    EXPECT_EQ(11, table->numRows());
    table->deleteSelectedRows();
    EXPECT_EQ(10, table->numRows());

    table->selectColumn(3);
    table->setPlotDesignation(Makhber::Z);
    EXPECT_EQ(table->colPlotDesignation(3), Makhber::Z);

    QStringList columnsList = table->columnsList();
    QStringList colNames = table->colNames();
    for (int i = 0; i < table->numCols(); i++) {
        EXPECT_EQ(table->column(i)->name(), colNames[i]);
        EXPECT_EQ(table->column(i)->name(), table->colLabel(i));
        EXPECT_EQ(table->name() + "_" + table->column(i)->name(), columnsList[i]);
    }

    EXPECT_FALSE(table->noXColumn());
    EXPECT_EQ(0, table->firstXCol());
    EXPECT_EQ(0, table->colX(1));
    EXPECT_EQ(0, table->colX(0));

    EXPECT_FALSE(table->noYColumn());
    EXPECT_EQ(1, table->colY(0));
    EXPECT_EQ(1, table->colY(1));
    EXPECT_EQ(4, table->colY(5));

    table->setNumRows(20);
    EXPECT_EQ(20, table->numRows());
    table->d_future_table->removeRows(15, 5);
    EXPECT_EQ(15, table->numRows());
    table->d_future_table->insertRows(10, 5);
    EXPECT_EQ(20, table->numRows());

    table->setNumCols(3);
    EXPECT_EQ(3, table->numCols());
    table->addCol();
    EXPECT_EQ(4, table->numCols());
    table->addColumns(3);
    EXPECT_EQ(7, table->numCols());
    table->insertCols(1, 2);
    EXPECT_EQ(9, table->numCols());
    table->addCol(Makhber::Z);
    EXPECT_EQ(10, table->numCols());

    table->selectColumn(4);
    table->setPlotDesignation(Makhber::xErr);
    EXPECT_EQ(1, table->selectedErrColumns().size());
    EXPECT_EQ(table->name() + "_" + table->column(4)->name(), table->selectedErrColumns()[0]);
    EXPECT_EQ(1, table->drawableColumnSelection().size());
    EXPECT_EQ(table->name() + "_" + table->column(4)->name(), table->drawableColumnSelection()[0]);

    EXPECT_EQ(table->colPlotDesignation(9), Makhber::Z);
    EXPECT_EQ(table->d_future_table->columnCount(Makhber::Z), 1);

    QList<int> expected_pds = { 1, 0, 0, 2, 4, 2, 0, 0, 0, 3 };
    EXPECT_EQ(expected_pds, table->plotDesignations());

    table->setColumnType(9, Makhber::ColumnMode::Text);
    EXPECT_EQ("", table->text(0, 9));
    table->setText(14, 9, "hello");
    EXPECT_EQ("hello", table->text(14, 9));
    table->setText(14, 9, "-1.9");
    EXPECT_EQ(-1.9, table->cell(14, 9));
    table->setText(30, 10, "hello");
    EXPECT_EQ("", table->text(30, 10));
    table->setText(30, 9, "hello");
    EXPECT_EQ("", table->text(30, 9));

    EXPECT_FALSE(table->commentsEnabled());
    table->setColComment(0, "first column");
    EXPECT_EQ("first column", table->colComment(0));
    QStringList table_comments = { "first column", "", "", "", "", "", "", "", "", "" };
    EXPECT_EQ(table_comments, table->colComments());

    table->selectColumn(2);
    table->d_future_table->fillSelectedCellsWithRowNumbers();
    table->selectColumn(5);
    table->d_future_table->fillSelectedCellsWithRandomNumbers();
    for (int i = 0; i < table->numRows(); i++) {
        EXPECT_EQ(i + 1, table->cell(i, 2));
        EXPECT_GT(1.0, table->cell(i, 5));
        EXPECT_LT(0.0, table->cell(i, 5));
    }
    table->clearSelection();
    for (int i = 0; i < 30; i++) {
        EXPECT_EQ(0.0, table->cell(i, 5));
    }
    table->selectColumn(2);
    table->clearCol();
    for (int i = 0; i < 30; i++) {
        EXPECT_EQ(0.0, table->cell(i, 2));
    }

    QStringList colList {};
    colList << "2"
            << "5"
            << "9";
    table->removeCol(colList);
    EXPECT_EQ(7, table->numCols());

    // table->setNumericPrecision(4);

    table->setName("TableTest");
    EXPECT_EQ("TableTest", table->name());

    table->setWindowLabel("MyLabel");
    EXPECT_EQ("MyLabel", table->windowLabel());

    table->setCaptionPolicy(MyWidget::Name);
    EXPECT_EQ(MyWidget::Name, table->captionPolicy());
    table->setCaptionPolicy(MyWidget::Label);
    EXPECT_EQ(MyWidget::Label, table->captionPolicy());
    table->setCaptionPolicy(MyWidget::Both);
    EXPECT_EQ(MyWidget::Both, table->captionPolicy());

    Column *col1 = table->column("1");
    col1->setName("colX");
    EXPECT_EQ("colX", col1->name());
    EXPECT_EQ(0, table->colIndex("colX"));

    table->d_future_table->setColumnWidth(0, 120);
    EXPECT_EQ(120, table->columnWidth(0));

    table->setBackgroundColor(QColor(Qt::blue));
    EXPECT_EQ(QColor(Qt::blue), table->backgroundColor());

    table->setTextColor(QColor(Qt::darkRed));
    EXPECT_EQ(QColor(Qt::darkRed), table->textColor());

    Table *table2 = newTable(0, 0);
    table2->importASCII("ascii_table.txt", "\t", 0, false, false, false, false);
    EXPECT_EQ(30, table2->numRows());
    EXPECT_EQ(2, table2->numCols());
    std::array<double, 30> col2_values = { 0.763194,  0.611632,  0.0615707, 0.267595, 0.0790613,
                                           0.588472,  0.840689,  0.0635694, 0.88199,  0.253856,
                                           0.606594,  0.832936,  0.271041,  0.541593, 0.579907,
                                           0.519119,  0.635008,  0.149243,  0.549456, 0.345052,
                                           0.304015,  0.0152424, 0.762356,  0.219869, 0.610833,
                                           0.0475982, 0.0987806, 0.879489,  0.428782, 0.172931 };
    for (int i = 0; i < table2->numCols(); i++) {
        EXPECT_EQ(table2->column(i)->columnMode(), Makhber::ColumnMode::Numeric);
    }
    for (int i = 0; i < 30; i++) {
        EXPECT_EQ(i + 1, table2->cell(i, 0));
        EXPECT_EQ(col2_values[i], table2->cell(i, 1));
    }

    QStringList header { "colX", "colY" };
    table2->setHeader(header);
    EXPECT_EQ("colX", table2->column(0)->name());
    EXPECT_EQ("colY", table2->column(1)->name());

    QList<int> cols = { 0 };
    TableStatistics *tablestats = newTableStatistics(table2, TableStatistics::StatColumn, cols);
    EXPECT_EQ(table2->column(0)->name(), tablestats->text(0, 0));
    EXPECT_EQ("[1:30]", tablestats->text(0, 1));
    EXPECT_EQ(15.5, tablestats->cell(0, 2));
    EXPECT_EQ(8.8034084308295046, tablestats->cell(0, 3));
    EXPECT_NEAR(77.5, tablestats->cell(0, 4), 2.e-14);
    EXPECT_EQ(465, tablestats->cell(0, 5));
    EXPECT_EQ(30, tablestats->cell(0, 6));
    EXPECT_EQ(30, tablestats->cell(0, 7));
    EXPECT_EQ(1, tablestats->cell(0, 8));
    EXPECT_EQ(1, tablestats->cell(0, 9));
    EXPECT_EQ(30, tablestats->cell(0, 10));
    tablestats->renameCol("Col", "Column");
    tablestats->removeCol("N");

    QList<int> rows = { 14 };
    TableStatistics *tablestats_rows = newTableStatistics(table2, TableStatistics::StatRow, rows);
    EXPECT_EQ(15, tablestats_rows->cell(0, 0));
    EXPECT_EQ(2, tablestats_rows->cell(0, 1));
    EXPECT_EQ(7.7899535, tablestats_rows->cell(0, 2));
    EXPECT_EQ(10.196545545640666, tablestats_rows->cell(0, 3));
    EXPECT_EQ(103.9695410643245, tablestats_rows->cell(0, 4));
    EXPECT_EQ(15.579907, tablestats_rows->cell(0, 5));
    EXPECT_EQ(15, tablestats_rows->cell(0, 6));
    EXPECT_EQ(0.579907, tablestats_rows->cell(0, 7));
    EXPECT_EQ(2, tablestats_rows->cell(0, 8));

    table2->setCommand(1, "2*col(2)");
    EXPECT_EQ("2*col(2)", table2->column(1)->formula(0));
    EXPECT_EQ("2*col(2)", table2->column(1)->formula(29));
    EXPECT_EQ(table2->column(1)->formula(30), "");
    table2->recalculate(1, false);
    for (int i = 0; i < 30; i++) {
        EXPECT_EQ(2 * col2_values[i], table2->cell(i, 1));
    }
    table2->selectColumn(1);
    table2->recalculate();
    for (int i = 0; i < 30; i++) {
        EXPECT_EQ(4 * col2_values[i], table2->cell(i, 1));
    }
    table2->setCell(0, 1, 2.0);
    EXPECT_EQ(2.0, table2->cell(0, 1));
    table2->setCell(0, 2, 2.0);
    EXPECT_EQ(0.0, table2->cell(0, 2));
    table2->clearCell(15, 1);
    EXPECT_EQ(0.0, table2->cell(15, 1));
    table2->setCell(29, 1, -7.0);
    EXPECT_EQ(-7.0, table2->cell(29, 1));
    table2->setCell(60, 1, 2.0);
    EXPECT_EQ(0.0, table2->cell(60, 1));

    table2->selectAllTable();
    bool export_success = table2->exportASCII("ascii_table2.txt", "\t", true);
    EXPECT_TRUE(export_success);
    export_success = table2->exportASCII("ascii_table2.txt", "\t", true, true);
    EXPECT_TRUE(export_success);

    table2->exportPDF("table.pdf");

    table2->addColumns(3);
    table2->setColumnType(2, Makhber::ColumnMode::DateTime);
    EXPECT_EQ(Makhber::ColumnMode::DateTime, table2->columnType(2));
    table2->setColumnType(3, Makhber::ColumnMode::Month);
    EXPECT_EQ(Makhber::ColumnMode::Month, table2->columnType(3));
    table2->setColumnType(4, Makhber::ColumnMode::Day);
    EXPECT_EQ(Makhber::ColumnMode::Day, table2->columnType(4));
    QList<Makhber::ColumnMode> expected_cms = {
        Makhber::ColumnMode::Numeric, Makhber::ColumnMode::Numeric, Makhber::ColumnMode::DateTime,
        Makhber::ColumnMode::Month, Makhber::ColumnMode::Day
    };
    EXPECT_EQ(expected_cms, table2->columnTypes());
    QList<Makhber::ColumnMode> col_modes = {
        Makhber::ColumnMode::Day, Makhber::ColumnMode::Month, Makhber::ColumnMode::Text,
        Makhber::ColumnMode::DateTime, Makhber::ColumnMode::Numeric
    };
    table2->setColumnTypes(col_modes);
    EXPECT_EQ(col_modes, table2->columnTypes());

    table->clear();
    table->copy(table2);
    EXPECT_EQ(30, table->numRows());
    EXPECT_EQ(5, table->numCols());

    projectname = "table.mkbr";
    saveProject();
    open("table.mkbr");

    Table *table3 = newTable("ascii_table.txt", "\t", 0, false, true, false, true, QLocale::c());
    EXPECT_EQ(30, table3->numRows());
    EXPECT_EQ(2, table3->numCols());
    for (int i = 0; i < 30; i++) {
        EXPECT_EQ(i + 1, table3->cell(i, 0));
        EXPECT_EQ(col2_values[i], table3->cell(i, 1));
    }
    table3->setCommands(QString("col(1)\tcol(2)"));

    table3->selectColumn(1);
    table3->copySelection();
    table3->addCol();
    table3->selectColumn(2);
    table3->pasteSelection();
    for (int i = 0; i < 30; i++) {
        EXPECT_EQ(col2_values[i], table3->cell(i, 2));
    }
    table3->cutSelection();
    table3->addCol();
    table3->selectColumn(3);
    table3->pasteSelection();
    for (int i = 0; i < 30; i++) {
        EXPECT_EQ(0.0, table3->cell(i, 2));
        EXPECT_EQ(col2_values[i], table3->cell(i, 3));
    }

    EXPECT_EQ("", table3->columnFormat(1));

    int windows = currentFolder()->windowCount();
    currentFolder()->removeWindow(table3);
    EXPECT_EQ(windows - 1, currentFolder()->windowCount());
}

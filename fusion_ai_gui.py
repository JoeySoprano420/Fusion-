import tkinter as tk
from hybrid_ai import hybrid_suggest
import pandas as pd
import matplotlib.pyplot as plt
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

class FusionLogWatcher(FileSystemEventHandler):
    def __init__(self, log_path):
        self.log_path = log_path

    def on_modified(self, event):
        if event.src_path.endswith('fusion_mode_transitions.csv'):
            df = pd.read_csv(self.log_path)
            df['Timestamp'] = pd.to_datetime(df['Timestamp'])
            df.plot(x='Timestamp', y='MemoryDelta(KB)', title='Memory Delta Over Time')
            plt.pause(0.01)

log_path = "fusion_mode_transitions.csv"
event_handler = FusionLogWatcher(log_path)
observer = Observer()
observer.schedule(event_handler, path='.', recursive=False)
observer.start()

try:
    while True: pass
except KeyboardInterrupt:
    observer.stop()
observer.join()

class AIEditorApp:
    def __init__(self, master):
        self.master = master
        master.title("Fusion++ AI Intent Manager")

        self.input = tk.Text(master, height=5, width=60)
        self.input.pack()

        self.generate_btn = tk.Button(master, text="🧠 Generate", command=self.generate)
        self.generate_btn.pack()

        self.output = tk.Text(master, height=20, width=80)
        self.output.pack()

    def generate(self):
        intent = self.input.get("1.0", tk.END).strip()
        suggestions = []
        for line in intent.split("\n"):
            if not line.strip(): continue
            print(f"[Intent]: {line}")
            code = hybrid_suggest(line.strip())#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QTextDocument>

class FppHighlighter : public QSyntaxHighlighter {
public:
    FppHighlighter(QTextDocument* parent) : QSyntaxHighlighter(parent) {
        // Keyword highlighting (let, func, print, etc.)
        QTextCharFormat keywordFormat;
        keywordFormat.setForeground(Qt::darkBlue);
        keywordFormat.setFontWeight(QFont::Bold);
        QStringList keywords = {"let", "func", "endfunc", "print", "emit", "asm", "derive"};
        for (const QString& k : keywords)
            rules.append({QRegularExpression("\\b"+k+"\\b"), keywordFormat});

        // String highlighting
        QTextCharFormat stringFormat;
        stringFormat.setForeground(Qt::darkGreen);
        rules.append({QRegularExpression("\".*\""), stringFormat});
    }

protected:
    void highlightBlock(const QString& text) override {
        for (const HighlightRule& r : rules) {
            QRegularExpressionMatchIterator i = r.pattern.globalMatch(text);
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                setFormat(match.capturedStart(), match.capturedLength(), r.format);
            }
        }
    }
private:
    struct HighlightRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightRule> rules;
};

fppEdit = new QTextEdit;
highlighter = new FppHighlighter(fppEdit->document());

// --- AI Prompt Tab ---
QWidget* aiTab = new QWidget;
QLineEdit* aiPromptEdit = new QLineEdit;
QPushButton* aiGenBtn = new QPushButton("Generate");
QTextEdit* aiOutput = new QTextEdit;
aiOutput->setReadOnly(true);

QHBoxLayout* aiInputLayout = new QHBoxLayout;
aiInputLayout->addWidget(aiPromptEdit);
aiInputLayout->addWidget(aiGenBtn);

QVBoxLayout* aiTabLayout = new QVBoxLayout;
aiTabLayout->addLayout(aiInputLayout);
aiTabLayout->addWidget(aiOutput);
aiTab->setLayout(aiTabLayout);

tabs->addTab(aiTab, "AI Generator");

connect(aiGenBtn, &QPushButton::clicked, [=]() {
    QString prompt = aiPromptEdit->text().trimmed();
    if (prompt.isEmpty()) return;
    aiGenBtn->setEnabled(false);
    aiOutput->append("> " + prompt);

    // You can use QProcess to run a python script or REST to a backend
    QProcess process;
    process.start("python", QStringList() << "ai_bridge.py" << prompt);
    process.waitForFinished();
    QString result = process.readAllStandardOutput();
    aiOutput->append(result);
    aiGenBtn->setEnabled(true);
});

// --- AI Prompt Tab ---
QWidget* aiTab = new QWidget;
QLineEdit* aiPromptEdit = new QLineEdit;
QPushButton* aiGenBtn = new QPushButton("Generate");
QTextEdit* aiOutput = new QTextEdit;
aiOutput->setReadOnly(true);

QHBoxLayout* aiInputLayout = new QHBoxLayout;
aiInputLayout->addWidget(aiPromptEdit);
aiInputLayout->addWidget(aiGenBtn);

QVBoxLayout* aiTabLayout = new QVBoxLayout;
aiTabLayout->addLayout(aiInputLayout);
aiTabLayout->addWidget(aiOutput);
aiTab->setLayout(aiTabLayout);

tabs->addTab(aiTab, "AI Generator");

connect(aiGenBtn, &QPushButton::clicked, [=]() {
    QString prompt = aiPromptEdit->text().trimmed();
    if (prompt.isEmpty()) return;
    aiGenBtn->setEnabled(false);
    aiOutput->append("> " + prompt);

    // You can use QProcess to run a python script or REST to a backend
    QProcess process;
    process.start("python", QStringList() << "ai_bridge.py" << prompt);
    process.waitForFinished();
    QString result = process.readAllStandardOutput();
    aiOutput->append(result);
    aiGenBtn->setEnabled(true);
});


            suggestions.append(f"// Intent: {line}\n{code}\n")
        self.output.delete("1.0", tk.END)
        self.output.insert(tk.END, "\n---\n".join(suggestions))

root = tk.Tk()
app = AIEditorApp(root)
root.mainloop()


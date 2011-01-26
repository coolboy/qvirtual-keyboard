#pragma once

class TraceObject{
public:
	TraceObject():m_weight(0){ }

	TraceObject(const QString& chr):m_weight(0), m_chr(chr){}

	QString getCharcter() const { return m_chr; }
	void setCharcter(const QString& val) { m_chr = val; }
	int getWeight() const { return m_weight; }
	void setWeight(int val) { m_weight = val; }

private:
	QString m_chr;
	int m_weight;
};
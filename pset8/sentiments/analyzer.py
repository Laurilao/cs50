import nltk

class Analyzer():
    """Implements sentiment analysis."""

    def __init__(self, positives, negatives):
        """Initialize Analyzer."""

        # add words from positive-words.txt
        self.positives = set()
        file = open("positive-words.txt", "r")
        for line in file:
            if not line.startswith(";"):
                self.positives.add(line.strip())
        file.close()
        
        # add words from negative-words.txt
        self.negatives = set()
        file = open("negative-words.txt", "r")
        for line in file:
            if not line.startswith(";"):
                self.negatives.add(line.strip())
        file.close()
        
    def analyze(self, text):
        """Analyze text for sentiment, returning its score."""
        
        score = 0
        
        tokenizer = nltk.tokenize.TweetTokenizer()
        tokens = tokenizer.tokenize(text)
        
        for token in tokens:
            token.lower()
            if token in self.positives:
                score += 1
            elif token in self.negatives:
                score -= 1
            else:
                score += 0
        
        return score

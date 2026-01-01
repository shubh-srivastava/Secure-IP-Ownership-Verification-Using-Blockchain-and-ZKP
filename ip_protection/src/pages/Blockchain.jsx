import { useEffect, useState } from "react";

export default function Blockchain() {
  const [chain, setChain] = useState([]);

  useEffect(() => {
    fetch("http://localhost:18080/chain")
      .then(r => r.json())
      .then(setChain);
  }, []);

  return (
    <div className="max-w-4xl mx-auto mt-10">
      <h2 className="text-2xl font-bold mb-4">Blockchain Explorer</h2>

      {chain.map((b, i) => (
        <div key={i} className="bg-white p-4 rounded shadow mb-3">
          <p><strong>Index:</strong> {b.index}</p>
          <p><strong>Creator:</strong> {b.creator}</p>
          <p className="break-all"><strong>IP Hash:</strong> {b.ipHash}</p>
          <p className="break-all"><strong>Prev Hash:</strong> {b.prevHash}</p>
          <p className="text-sm text-gray-600">
            {new Date(b.timestamp * 1000).toLocaleString()}
          </p>
        </div>
      ))}
    </div>
  );
}
